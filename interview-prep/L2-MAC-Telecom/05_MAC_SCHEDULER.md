# 🎯 MAC Scheduler - Complete Deep Dive

## 🎯 Why Scheduler Critical?

**MAC Scheduler decides:**
- Konsa UE schedule karna hai
- Kitni resources dene hai
- Konsa MCS use karna hai
- Kab schedule karna hai

**Direct impact:**
- Throughput
- Latency
- Fairness
- QoS guarantees
- Battery life

---

## 📊 Scheduler Architecture

```
┌────────────────────────────────────┐
│         Input Collection           │
│  - BSR from UE                     │
│  - CQI/RI/PMI                      │
│  - PHR                             │
│  - HARQ status                     │
└──────────┬─────────────────────────┘
           │
┌──────────▼─────────────────────────┐
│      Resource Manager              │
│  - Available PRBs                  │
│  - Numerology                      │
│  - Bandwidth Parts                 │
└──────────┬─────────────────────────┘
           │
┌──────────▼─────────────────────────┐
│      Decision Engine               │
│  - Priority calculation            │
│  - QoS handling                    │
│  - Algorithm (PF/RR/MaxCI)         │
└──────────┬─────────────────────────┘
           │
┌──────────▼─────────────────────────┐
│      Output Generator              │
│  - DCI generation                  │
│  - Grant scheduling                │
│  - HARQ assignment                 │
└────────────────────────────────────┘
```

---

## 🎯 Scheduling Algorithms

### **1. Round Robin (RR)**

**Concept:** Sab UEs ko baari baari resources

```c
ue_t *select_next_ue_rr() {
    static int last_index = 0;
    int next = (last_index + 1) % num_ues;
    last_index = next;
    return &ues[next];
}
```

**Pros:**
- ✅ Fair (sab ko equal chance)
- ✅ Simple to implement
- ✅ Predictable

**Cons:**
- ❌ Channel condition ignore
- ❌ Throughput suboptimal
- ❌ Bad UE waste resources

### **2. Max C/I (Best Channel)**

**Concept:** Sabse acche channel wala UE ko priority

```c
ue_t *select_max_ci() {
    ue_t *best = NULL;
    int max_cqi = 0;
    for (int i = 0; i < num_ues; i++) {
        if (ues[i].cqi > max_cqi) {
            max_cqi = ues[i].cqi;
            best = &ues[i];
        }
    }
    return best;
}
```

**Pros:**
- ✅ Maximum throughput
- ✅ Efficient resource use

**Cons:**
- ❌ Cell-edge UEs starve
- ❌ Unfair
- ❌ QoS violations

### **3. Proportional Fair (PF)** ⭐ MOST USED

**Concept:** Throughput aur fairness balance

**Formula:**
```
Priority = Instantaneous_Rate / Average_Rate
         = R_inst(i,t) / R_avg(i,t-1)
```

**Implementation:**
```c
double pf_metric(ue_t *ue) {
    // Calculate instantaneous rate from CQI
    double r_inst = cqi_to_rate(ue->cqi);
    
    // Average rate (exponential moving avg)
    double r_avg = ue->avg_rate;
    
    // Priority metric
    return r_inst / r_avg;
}

ue_t *select_pf() {
    ue_t *best = NULL;
    double max_metric = 0;
    
    for (int i = 0; i < num_ues; i++) {
        if (ues[i].has_data) {
            double metric = pf_metric(&ues[i]);
            if (metric > max_metric) {
                max_metric = metric;
                best = &ues[i];
            }
        }
    }
    
    // Update average rate
    if (best) {
        update_avg_rate(best);
    }
    
    return best;
}

void update_avg_rate(ue_t *ue) {
    // Exponential moving average
    double alpha = 0.1;  // Smoothing factor
    ue->avg_rate = (1 - alpha) * ue->avg_rate + 
                   alpha * ue->current_rate;
}
```

**Pros:**
- ✅ Good throughput
- ✅ Fair to all UEs
- ✅ Cell-edge UEs not starved

**Cons:**
- ❌ More complex
- ❌ Doesn't directly handle QoS

### **4. QoS-Aware (QCI-Based)**

**Concept:** QCI (QoS Class Identifier) priority

**LTE QCI Values:**
| QCI | Resource | Priority | Use Case |
|-----|----------|----------|----------|
| 1 | GBR | 2 | VoIP |
| 2 | GBR | 4 | Video Call |
| 3 | GBR | 3 | Real-time gaming |
| 4 | GBR | 5 | Video streaming |
| 5 | Non-GBR | 1 | IMS Signaling |
| 6 | Non-GBR | 6 | Video, TCP-based |
| 7 | Non-GBR | 7 | Voice, Video |
| 8 | Non-GBR | 8 | Default bearer |
| 9 | Non-GBR | 9 | Default bearer |

**Implementation:**
```c
ue_t *select_qos_aware() {
    // Priority order:
    // 1. GBR not satisfied
    // 2. High priority QCI
    // 3. PF metric for ties
    
    ue_t *best = NULL;
    int best_priority = INT_MAX;
    
    for (int i = 0; i < num_ues; i++) {
        ue_t *ue = &ues[i];
        
        // Check GBR satisfaction
        if (ue->gbr_required && !ue->gbr_satisfied) {
            return ue;  // Highest priority
        }
        
        if (ue->priority < best_priority) {
            best_priority = ue->priority;
            best = ue;
        }
    }
    
    return best;
}
```

---

## 🎯 Resource Allocation

### **PRB (Physical Resource Block):**

**LTE:**
- 1 PRB = 12 subcarriers × 7 OFDM symbols
- Bandwidth: 180 kHz
- Time: 0.5 ms (1 slot)
- 1 ms (TTI) = 2 slots

**5G NR:**
- Numerology dependent
- μ=0: 15 kHz SCS, slot=1ms
- μ=1: 30 kHz SCS, slot=0.5ms
- μ=2: 60 kHz SCS, slot=0.25ms
- μ=3: 120 kHz SCS, slot=0.125ms

### **Frequency Domain Allocation:**

**Type 0 (Bitmap):**
- RBG (Resource Block Group)
- Bitmap indicates allocated RBGs
- Less DCI overhead

**Type 1 (Subset):**
- Specific PRBs
- More flexibility
- More DCI overhead

**Type 2 (Contiguous):**
- Continuous PRBs
- Simplest
- Most common

### **MCS Selection:**

**MCS = Modulation and Coding Scheme**

LTE MCS (0-31):
- MCS 0-9: QPSK
- MCS 10-16: 16QAM
- MCS 17-28: 64QAM

5G NR MCS (0-31):
- QPSK (0-4)
- 16QAM (5-10)
- 64QAM (11-19)
- 256QAM (20-27)

**Selection based on CQI:**
```c
int select_mcs(int cqi, int target_bler) {
    // Map CQI to MCS (table lookup)
    // Consider HARQ retransmission
    
    int base_mcs = cqi_to_mcs_table[cqi];
    
    // Adjust for target BLER (typically 10%)
    if (current_bler > target_bler) {
        base_mcs--;  // More robust
    } else if (current_bler < target_bler / 2) {
        base_mcs++;  // More aggressive
    }
    
    return clamp(base_mcs, 0, 28);
}
```

---

## 🎯 Scheduling Time Domain

### **LTE TDD Configurations:**

```
DL : Downlink
UL : Uplink
S  : Special

Config 0: D S U U U D S U U U
Config 1: D S U U D D S U U D
Config 2: D S U D D D S U D D
Config 3: D S U U U D D D D D
...
```

### **5G NR Slot Format:**

```
Slot = 14 OFDM symbols
- All DL
- All UL
- Mix (DDDDDDSDU)

Mini-slot (URLLC):
- 2-7 symbols
- Lower latency
- For URLLC, eMBB
```

### **TTI (Transmission Time Interval):**

LTE: 1 ms (fixed)
5G NR: Flexible
- 1 slot
- 2-7 symbols (mini-slot)
- 14 symbols (slot)
- Aggregated slots

---

## 🎯 UL vs DL Scheduling

### **DL Scheduling:**
- eNB knows DL queue
- Direct allocation
- Easier

### **UL Scheduling:**
- UE has data
- BSR informs eNB
- 4ms delay (LTE)
- More complex

```
UL Scheduling:
┌──────┐                ┌──────┐
│  UE  │                │ eNB  │
└──┬───┘                └──┬───┘
   │   BSR (n)             │
   ├──────────────────────→│
   │                       │ Schedule
   │   UL Grant (n+4)      │
   │←──────────────────────│
   │                       │
   │   Data (n+8)          │
   ├──────────────────────→│
   │                       │
```

---

## 🎯 Real Interview Questions

### **Q1: Aap MAC scheduler kaise design karte ho?**

**Answer:**
"Maine 5 saal MAC scheduler design kiya hai. My approach:

**Architecture:**
```
Input → Decision → Allocation → Output
```

**Key Components:**

**1. Input Manager:**
- BSR collection
- CQI processing
- PHR analysis
- HARQ status tracking

**2. Algorithm Engine:**
- Configurable: PF, RR, MaxCI
- QoS handling per QCI
- Priority calculation

**3. Resource Allocator:**
- PRB allocation
- MCS selection
- HARQ process assignment
- TBS calculation

**4. DCI Generator:**
- Format selection
- Grant scheduling
- Output to PHY

**Performance Targets:**
- < 1ms decision time
- 95%+ resource utilization
- Fairness >0.85 (Jain's index)
- QoS guarantees met

**My Achievement:**
- 25% latency reduction
- Implemented PF with QoS overlay
- Optimized for 100+ UEs"

### **Q2: Proportional Fair algorithm explain karo?**

**Answer:**
"Proportional Fair (PF) throughput aur fairness balance karta hai:

**Formula:**
```
Priority(i) = R_inst(i) / R_avg(i)
```

**R_inst:** Instantaneous rate (current channel)
**R_avg:** Average past throughput

**Why it works?**

Case 1: Good channel UE
- R_inst high
- If served often, R_avg increases
- Metric drops → others get chance

Case 2: Bad channel UE
- R_inst low
- R_avg also low
- Metric balanced → fair share

**Implementation:**
```c
double pf_priority(ue_t *ue) {
    double r_inst = calc_rate_from_cqi(ue->cqi);
    double r_avg = ue->avg_rate;
    
    // Avoid division by zero
    if (r_avg < EPSILON) r_avg = EPSILON;
    
    return r_inst / r_avg;
}

void update_avg_rate(ue_t *ue, bool was_scheduled) {
    double alpha = 0.1;  // smoothing
    double r_now = was_scheduled ? ue->actual_rate : 0;
    ue->avg_rate = (1 - alpha) * ue->avg_rate + 
                   alpha * r_now;
}
```

**Optimal alpha:**
- Small alpha (0.01-0.1): Smoother, slower adaptation
- Large alpha (0.3-0.5): Quick adaptation, oscillation"

### **Q3: HARQ aur scheduler kaise interact karte hain?**

**Answer:**
"Scheduler aur HARQ closely tied hain:

**Scheduler considers:**
1. **HARQ process availability**
2. **Retransmissions priority** (over new data)
3. **Soft buffer status**
4. **HARQ feedback** (ACK/NACK)

**Flow:**
```c
void schedule_dl(ue_t *ue) {
    // Step 1: Check HARQ retransmissions
    for (int p = 0; p < 8; p++) {
        if (ue->harq[p].pending_retx) {
            schedule_retransmission(ue, p);
            return;  // Retx priority
        }
    }
    
    // Step 2: Find free HARQ process
    int free_proc = find_free_harq_process(ue);
    if (free_proc == -1) {
        return;  // No process available
    }
    
    // Step 3: Schedule new data
    if (ue->dl_buffer > 0) {
        ue->harq[free_proc].active = true;
        ue->harq[free_proc].rv = 0;  // RV0 for new
        schedule_new_transmission(ue, free_proc);
    }
}
```

**On HARQ feedback:**
```c
void on_harq_feedback(ue_t *ue, int proc, bool ack) {
    if (ack) {
        // Free the process
        ue->harq[proc].active = false;
        ue->harq[proc].retx_count = 0;
    } else {
        // Mark for retransmission
        ue->harq[proc].pending_retx = true;
        ue->harq[proc].retx_count++;
        
        if (ue->harq[proc].retx_count >= MAX_RETX) {
            // Declare failure to RLC AM
            rlc_notify_failure(ue, proc);
            ue->harq[proc].active = false;
        } else {
            // Increment redundancy version
            ue->harq[proc].rv = next_rv(ue->harq[proc].rv);
        }
    }
}
```"

### **Q4: Apne project mein 25% latency reduction kaise achieve kiya?**

**Answer:**
"My optimization approach:

**1. Profiling:**
- Identified hotspots in scheduling code
- Found bottlenecks in:
  - CQI to MCS lookup (linear search)
  - HARQ process search
  - Priority calculation

**2. Optimizations Applied:**

**a) Lookup Table Optimization:**
```c
// Before: Linear search
int find_mcs(int cqi) {
    for (int i = 0; i < 32; i++) {
        if (mcs_table[i].cqi >= cqi) return i;
    }
}

// After: Direct array access
static const int cqi_to_mcs[16] = {0, 2, 4, 6, ...};
int find_mcs(int cqi) {
    return cqi_to_mcs[cqi];  // O(1)
}
```

**b) HARQ Process Bitmap:**
```c
// Before: Loop through all processes
int find_free_harq(ue_t *ue) {
    for (int i = 0; i < 8; i++) {
        if (!ue->harq[i].active) return i;
    }
    return -1;
}

// After: Bitmap + bit manipulation
int find_free_harq(ue_t *ue) {
    if (ue->harq_active_mask == 0xFF) return -1;
    return __builtin_ctz(~ue->harq_active_mask);
}
```

**c) Cached Metrics:**
- Cache PF metrics per TTI
- Recalculate only when CQI changes
- Reduced CPU by 15%

**d) Lock-free Data Structures:**
- Used atomic operations
- Removed mutex contention
- Better SMP performance

**e) Algorithm Refinement:**
- Pre-screen UEs before priority calc
- Skip UEs with no data
- Skip UEs in DRX sleep

**Result:**
- 25% latency reduction
- Better real-time guarantees
- Same hardware, more capacity"

### **Q5: Logical Channel Prioritization (LCP) kaise kaam karta hai?**

**Answer:**
"LCP UE-side mechanism hai jab UL grant milta hai:

**Two-Phase Allocation:**

**Phase 1: Token Bucket (PBR-based)**
```c
// Each LC has:
// PBR: Prioritized Bit Rate
// BSD: Bucket Size Duration
// Bj: Bucket level (filled at PBR rate)

void update_buckets(double delta_t) {
    for (lc in logical_channels) {
        Bj[lc] += PBR[lc] * delta_t;
        if (Bj[lc] > BSD[lc] * PBR[lc]) {
            Bj[lc] = BSD[lc] * PBR[lc];  // Cap
        }
    }
}

// Phase 1 allocation
for (lc in priority_order) {
    int allocated = min(Bj[lc], available_grant);
    fill_pdu_with_lc(lc, allocated);
    Bj[lc] -= allocated;
    available_grant -= allocated;
}
```

**Phase 2: Strict Priority**
```c
// After Phase 1, allocate by strict priority
for (lc in priority_order) {
    if (available_grant == 0) break;
    
    int more = min(buffer[lc], available_grant);
    fill_pdu_with_lc(lc, more);
    available_grant -= more;
}
```

**Example:**
```
LC1: priority=1, PBR=200, buffer=1000
LC2: priority=2, PBR=100, buffer=500
Grant: 600 bytes

Phase 1:
- LC1 gets 200 (PBR)
- LC2 gets 100 (PBR)
- Remaining: 300 bytes

Phase 2:
- LC1 (high priority) gets remaining 300
- Total: LC1=500, LC2=100
```

**Why two phases?**
- Phase 1 ensures minimum QoS
- Phase 2 maximizes priority"

### **Q6: BSR (Buffer Status Report) kya hai?**

**Answer:**
"BSR UE → eNB MAC CE jo UL data status batata hai.

**Why needed?**
- eNB ko nahi pata UE ke paas kitna data hai
- Without BSR, no UL grant
- Critical for UL scheduling

**4 Logical Channel Groups (LCG):**
- LCG 0: Highest priority (e.g., signaling)
- LCG 1: VoLTE
- LCG 2: Video
- LCG 3: Best effort

**BSR Types:**

**1. Long BSR (4 LCGs):**
```
┌──────────┬──────────┬──────────┬──────────┐
│  LCG0    │  LCG1    │  LCG2    │  LCG3    │
│ Buffer   │ Buffer   │ Buffer   │ Buffer   │
└──────────┴──────────┴──────────┴──────────┘
```

**2. Short BSR (1 LCG):**
```
┌──────┬──────────┐
│ LCG  │ Buffer   │
└──────┴──────────┘
```

**3. Truncated BSR:**
- When PDU full, can't fit Long BSR
- Send 1 LCG (highest priority data)

**4. Padding BSR:**
- When MAC PDU has padding
- Use padding to send BSR

**Triggers:**
```c
void check_bsr_trigger() {
    if (new_data_arrived_higher_priority) {
        trigger_regular_bsr();
    }
    if (data_arrived_when_buffer_empty) {
        trigger_regular_bsr();
    }
    if (periodic_bsr_timer_expired) {
        trigger_periodic_bsr();
    }
    if (mac_pdu_has_padding) {
        trigger_padding_bsr();
    }
}
```

**Buffer Levels (encoded):**
- 6-bit value (Short)
- 8-bit value (Long, NR)
- Logarithmic mapping
- Range: 0 to >150 KB"

### **Q7: 5G NR mini-slot kya hai?**

**Answer:**
"5G NR mini-slot URLLC ke liye introduce hua:

**Concept:**
- Slot = 14 OFDM symbols
- Mini-slot = 2-13 symbols
- Lower latency

**Use Cases:**
1. **URLLC** (Industrial automation)
2. **Latency-sensitive** apps
3. **Resource sharing** with eMBB

**Comparison:**
```
LTE TTI: 1 ms (fixed)
5G NR slot: 1ms / 0.5ms / 0.25ms / 0.125ms (numerology)
5G NR mini-slot: 2-symbol = 0.143 ms (15kHz SCS)
                 2-symbol = 0.071 ms (30kHz SCS)
```

**Scheduling:**
- Pre-emption supported
- URLLC traffic can pre-empt eMBB
- Indication via PI-RNTI

**Implementation:**
```c
typedef struct {
    int start_symbol;    // 0-13
    int num_symbols;     // 2-14
    int numerology;      // 0-3
    double slot_duration_ms;
} time_allocation_t;

time_allocation_t schedule_urllc(ue_t *ue) {
    time_allocation_t alloc;
    
    if (ue->traffic_type == URLLC) {
        alloc.num_symbols = 2;  // Mini-slot
        alloc.numerology = 1;   // 30 kHz
        // Total: 0.07 ms latency
    } else {
        alloc.num_symbols = 14;  // Full slot
        alloc.numerology = 0;
    }
    
    return alloc;
}
```

**Benefits:**
- 14x lower latency than LTE
- Critical for autonomous vehicles
- Industrial IoT support"

### **Q8: Carrier Aggregation me scheduling kaise change hoti hai?**

**Answer:**
"CA me multiple carriers parallel use hote hain:

**Scheduling Considerations:**

**1. Cross-Carrier Scheduling:**
```
PCC (Primary)        SCC (Secondary)
   ↓                       ↓
DCI (allocates SCC)   Data on SCC
```

**2. Same-Carrier Scheduling:**
```
Each cc independent:
PCC: DCI + Data
SCC: DCI + Data
```

**3. Load Balancing:**
```c
void schedule_ca_ue(ue_t *ue) {
    int best_cc = -1;
    double best_metric = 0;
    
    for (cc in active_carriers) {
        double metric = pf_metric(ue, cc);
        // Consider load balancing
        metric *= load_factor(cc);
        
        if (metric > best_metric) {
            best_metric = metric;
            best_cc = cc;
        }
    }
    
    schedule_on_carrier(ue, best_cc);
}
```

**4. HARQ across CCs:**
- Each CC has independent HARQ
- 8 processes per CC (LTE)
- Total: 8 × num_CCs

**5. PUCCH:**
- HARQ-ACK can be on PCC PUCCH
- Or distributed across PUSCHs"

---

## ✅ Self-Test

1. ✅ Scheduler ke 4 main inputs?
2. ✅ PF algorithm ka formula?
3. ✅ Round Robin vs PF differences?
4. ✅ MCS selection kaise hota hai?
5. ✅ HARQ aur scheduler interaction?
6. ✅ LCP ke 2 phases?
7. ✅ BSR ke 4 types?
8. ✅ Mini-slot kya hai?
9. ✅ CA me scheduling differences?
10. ✅ 25% latency reduction kaise possible?

---

## 🚀 Next File:
**Read:** `06_CARRIER_AGGREGATION.md`
