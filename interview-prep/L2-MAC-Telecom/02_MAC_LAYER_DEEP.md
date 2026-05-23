# 🎯 MAC Layer - Complete Deep Dive

## 🎯 MAC Layer Overview

### **Position in Stack:**
```
┌──────────┐
│  PDCP    │ Layer 2 (top)
└────┬─────┘
     │
┌────▼─────┐
│   RLC    │ Layer 2 (middle)
└────┬─────┘
     │
┌────▼─────┐
│   MAC    │ Layer 2 (bottom) ⭐ TUMHARA EXPERTISE
└────┬─────┘
     │
┌────▼─────┐
│   PHY    │ Layer 1
└──────────┘
```

### **Main Functions of MAC Layer:**

1. **Mapping** - Logical channels → Transport channels
2. **Multiplexing/Demultiplexing** - Multiple logical channels into one transport block
3. **Scheduling** - UL/DL resource allocation ⭐
4. **HARQ** - Error correction with retransmission ⭐
5. **Random Access** - RACH procedure ⭐
6. **Logical Channel Prioritization (LCP)**
7. **Discontinuous Reception (DRX)**
8. **Padding** - Fill empty space in TB
9. **Transport Format Selection** - MCS selection

---

## 📊 MAC PDU Structure

### **MAC PDU Format:**

```
┌─────────────────────────────────────────────────┐
│                  MAC PDU                         │
├──────────┬──────────┬──────────┬───────────────┤
│ MAC      │ MAC SDU  │ MAC SDU  │  Padding      │
│ Header   │   1      │    2     │  (if any)     │
└──────────┴──────────┴──────────┴───────────────┘
```

### **MAC Header:**

```
┌──────┬──────┬──────┬──────────┐
│ R/F2 │  E   │ LCID │  L (opt) │ ← One subheader per SDU
└──────┴──────┴──────┴──────────┘

E = Extension bit (more subheaders follow)
LCID = Logical Channel ID (5 bits)
L = Length field (only for variable size)
```

### **MAC SDU Types:**

1. **Data MAC SDU** - Actual user/control data
2. **MAC Control Elements (MAC CE):**
   - BSR (Buffer Status Report)
   - PHR (Power Headroom Report)
   - C-RNTI MAC CE
   - DRX Command
   - Timing Advance Command
   - Activation/Deactivation (for CA)

---

## 🔄 HARQ Deep Dive (TUMHARI EXPERTISE)

### **What is HARQ?**

**HARQ = Hybrid Automatic Repeat reQuest**
- Combination of FEC (Forward Error Correction) + ARQ
- Operates at MAC layer
- Faster than RLC ARQ

### **Key Features:**

1. **8 Stop-and-Wait Processes** (LTE FDD)
2. **Soft Combining** - Failed transmissions combined with retransmissions
3. **Async UL** vs **Sync DL** in LTE
4. **All Async** in 5G NR

### **HARQ Process:**

```
Process 1:  TX → Wait ACK/NACK → If NACK, Retransmit
Process 2:  TX → Wait ACK/NACK → If NACK, Retransmit
Process 3:  TX → Wait ACK/NACK → If NACK, Retransmit
...
Process 8:  TX → Wait ACK/NACK → If NACK, Retransmit
```

**While process 1 is waiting, processes 2-8 transmit!**

### **HARQ Timing (LTE FDD):**

```
DL HARQ:
Subframe N:    eNB sends DL data (Process X)
Subframe N+4:  UE sends ACK/NACK on PUCCH/PUSCH

UL HARQ:
Subframe N:    eNB sends UL grant
Subframe N+4:  UE transmits UL data
Subframe N+8:  eNB sends ACK/NACK
Subframe N+12: UE retransmits if NACK
```

### **HARQ Combining Methods:**

1. **Chase Combining (CC):**
   - Same data retransmitted
   - Receiver combines bits
   - Improves SNR

2. **Incremental Redundancy (IR):**
   - Different redundancy versions sent
   - More robust than CC
   - Used in LTE/5G

### **Q: Aap HARQ ko kaise implement karte ho?**

**Answer:**
"Maine 5 saal se HARQ implement kiya hai. Implementation approach:

**1. Data Structures:**
```c
typedef struct harq_process {
    uint8_t process_id;        // 0-7
    bool is_active;
    uint8_t rv;                // Redundancy version (0,1,2,3)
    uint8_t ndi;               // New Data Indicator
    uint32_t tb_size;
    uint8_t *tb_buffer;        // Transport block
    uint8_t retx_count;
    uint32_t timer;
} harq_process_t;
```

**2. State Machine:**
- INIT → ACTIVE → WAITING_ACK → ACK/NACK → 
  Either DONE (ACK) or RETRANSMIT (NACK)

**3. NDI (New Data Indicator):**
- Toggles for new data
- Same value = retransmission

**4. Soft Buffer Management:**
- Each process maintains soft buffer
- Combine on retransmission

**5. Maximum Retransmissions:**
- Typically 3-4 attempts
- Then declare failure to RLC AM"

---

## 🎯 RACH Procedure Deep Dive

### **Two Types:**

1. **Contention-Based RACH (CBRA):**
   - Initial access
   - RRC connection re-establishment
   - Uplink data when out-of-sync

2. **Contention-Free RACH (CFRA):**
   - Handover
   - DL data arrival when UE out-of-sync
   - Uses dedicated preamble

### **CBRA (Contention-Based) - 4 Step:**

```
UE                          eNB/gNB
│                              │
│ Msg1: Preamble (PRACH)       │
├─────────────────────────────→│
│                              │
│ Msg2: RAR (Random Access     │
│       Response on PDSCH)     │
│←─────────────────────────────│
│                              │
│ Msg3: RRC Connection         │
│       Request (PUSCH)        │
├─────────────────────────────→│
│                              │
│ Msg4: Contention Resolution  │
│       (PDSCH)                │
│←─────────────────────────────│
```

### **Detailed Flow:**

#### **Msg1: Preamble**
- UE selects random preamble (1 of 64)
- Transmit on PRACH
- Specific time-frequency resource (RACH Configuration)

```
Preamble = Zadoff-Chu sequence
64 preambles per cell
Cyclic shifts for orthogonality
```

#### **Msg2: Random Access Response (RAR)**

Within RAR Window (3-10 ms):
```
RAR contains:
- RAPID (RA Preamble Identifier)
- TA (Timing Advance) command
- UL grant for Msg3
- Temporary C-RNTI (TC-RNTI)
- Backoff Indicator (if needed)
```

**If UE doesn't get RAR:**
- Increment preamble TX counter
- Power ramp up (PREAMBLE_POWER_RAMP_STEP)
- Try again
- Max attempts (preambleTransMax = 200)

#### **Msg3: Scheduled Transmission**

UE sends:
- For initial access: RRC Connection Request
- For RRC re-establishment: Re-establishment Request
- For handover: C-RNTI MAC CE
- HARQ enabled (4 retransmissions max)

#### **Msg4: Contention Resolution**

eNB sends:
- Contention Resolution Identity MAC CE
- Echoes UE's contention resolution identity
- If matches → UE wins contention
- If no match → UE loses, retry RACH

### **CFRA (Contention-Free) - 3 Step:**

```
UE                          eNB
│                            │
│ Msg0: Dedicated preamble   │
│       assignment (RRC)     │
│←───────────────────────────│
│                            │
│ Msg1: Dedicated Preamble   │
├───────────────────────────→│
│                            │
│ Msg2: RAR with grant       │
│←───────────────────────────│
```

### **2-Step RACH (5G NR Innovation):**

```
UE                          gNB
│                            │
│ MsgA: Preamble + Payload   │
├───────────────────────────→│
│                            │
│ MsgB: RAR + Resolution     │
│←───────────────────────────│
```

**Benefits:**
- Lower latency
- Better for mMTC, URLLC

### **Q: Aap RACH ko kaise implement karte ho?**

**Answer:**
"Mera **5G NR RACH Process Simulator** project hai - per 3GPP TS 38.321.

**Implementation approach:**

**1. State Machine Design:**
```c
typedef enum {
    RACH_INIT,
    RACH_PREAMBLE_TX,      // Msg1
    RACH_WAIT_RAR,         // Wait Msg2
    RACH_RAR_RECEIVED,
    RACH_MSG3_TX,          // Msg3
    RACH_WAIT_MSG4,        // Wait Msg4
    RACH_CONTENTION_RESOLVED, // Msg4 success
    RACH_FAILED
} rach_state_t;
```

**2. Preamble Selection:**
```c
uint8_t select_preamble() {
    // CFRA: dedicated preamble from RRC
    // CBRA: random selection from group A/B
    if (group_a_payload < threshold) {
        return random_select(GROUP_A_PREAMBLES);
    } else {
        return random_select(GROUP_B_PREAMBLES);
    }
}
```

**3. RAR Window:**
```c
// Start RAR window after preamble TX
ra_response_window_start = subframe + 3;
ra_response_window_end = ra_response_window_start + ra_window_size;
```

**4. Backoff Logic:**
```c
if (rar_received == false) {
    if (backoff_indicator > 0) {
        wait_time = random(0, BI_value);
    }
    if (++preamble_tx_counter > max_attempts) {
        report_failure_to_rrc();
    }
    // Power ramp up
    preamble_power += POWER_RAMP_STEP;
}
```

**5. Validation:**
- Validated against 3GPP spec
- Tested CBRA and CFRA flows
- Handled edge cases (timeouts, backoff)
- 25% latency improvement achieved"

---

## 📅 MAC Scheduler Deep Dive

### **Scheduler Decisions:**

The MAC scheduler decides:
1. **Which UE** to schedule (priority)
2. **How much resources** (PRBs)
3. **Which MCS** (Modulation Coding Scheme)
4. **HARQ process** to use
5. **TB size** (Transport Block Size)
6. **Logical channels** to multiplex

### **Scheduler Inputs:**

**From UE:**
- BSR (Buffer Status Report)
- PHR (Power Headroom Report)
- CQI (Channel Quality Indicator)
- RI (Rank Indicator)
- PMI (Precoding Matrix Indicator)

**From System:**
- Available PRBs
- HARQ process status
- QoS requirements
- DRX status

### **Common Scheduling Algorithms:**

**1. Round Robin:**
- Each UE gets turn
- Simple, fair
- Doesn't consider channel quality

**2. Max C/I (Channel Quality):**
- UE with best channel gets resources
- Maximum throughput
- Unfair to cell-edge UEs

**3. Proportional Fair (PF):**
- Balance throughput and fairness
- Most common in real systems
- Formula: `priority = current_rate / average_rate`

```c
double pf_metric(ue_t *ue) {
    double instantaneous_rate = calc_rate(ue->cqi);
    double average_rate = ue->avg_rate;
    return instantaneous_rate / average_rate;
}
```

**4. QoS-Aware:**
- Considers QCI (QoS Class Identifier)
- Priority-based
- For URLLC, eMBB, mMTC

### **Q: Aap MAC scheduler kaise design karte ho?**

**Answer:**
"Maine 5 saal MAC scheduler design kiya hai. My approach:

**1. Architecture:**
```
┌─────────────────────────────────┐
│      MAC Scheduler              │
├─────────────────────────────────┤
│  Input Manager                  │
│  - BSR collection               │
│  - CQI processing               │
│  - PHR analysis                 │
├─────────────────────────────────┤
│  Resource Manager               │
│  - PRB allocation               │
│  - HARQ tracking                │
├─────────────────────────────────┤
│  Decision Engine                │
│  - PF Algorithm                 │
│  - QoS prioritization           │
├─────────────────────────────────┤
│  Output Manager                 │
│  - DCI generation               │
│  - Grant scheduling             │
└─────────────────────────────────┘
```

**2. Key Design Points:**

a) **Modular design** - separate concerns
b) **Real-time performance** - <1ms decision
c) **Configurable algorithm** - PF, RR, etc.
d) **QoS handling** - per QCI
e) **HARQ aware** - prioritize retx

**3. Flow:**
```
Every TTI (1ms):
1. Collect inputs (BSR, CQI, PHR)
2. Update HARQ status
3. Identify UEs to schedule
4. Calculate priority metrics
5. Allocate PRBs
6. Select MCS
7. Generate DCI
8. Send grants
```

**4. Optimizations:**
- Cache CQI mappings
- Pre-compute MCS tables
- Avoid floating point in hot path
- Lock-free data structures

**5. Achieved 25% latency reduction** through algorithmic improvements."

---

## 🎯 LTE vs 5G NR MAC Differences

| Feature | LTE MAC | 5G NR MAC |
|---------|---------|-----------|
| TTI | 1 ms (fixed) | Flexible (numerology) |
| Subcarrier | 15 kHz | 15/30/60/120/240 kHz |
| HARQ | 8 processes (FDD) | Up to 16 processes |
| HARQ DL | Sync | Async |
| HARQ UL | Async | Async |
| RACH | 4-step only | 2-step + 4-step |
| BWP | No | Yes (Bandwidth Parts) |
| Beamforming | Limited | Native support |
| Mini-slot | No | Yes (URLLC) |

---

## 🎯 Real Interview Questions

### **Q1: MAC PDU structure kya hoti hai?**

Already covered above. Use the diagram!

### **Q2: HARQ process count kya hai? Kyun?**

**Answer:**
"LTE FDD mein 8 HARQ processes hote hain.

**Reason:**
- HARQ RTT (Round Trip Time) = 8 ms
- 4 ms for transmission and processing each way
- 8 processes ensure 100% utilization
- While 1 process waits, others transmit

5G NR mein up to 16 processes (more flexibility)."

### **Q3: BSR kab trigger hota hai?**

**Answer:**
"BSR ke trigger conditions:

1. **Regular BSR:**
   - New data arrival on higher priority LCG
   - Data on LCG (when no other data was there)

2. **Periodic BSR:**
   - Periodic timer expires
   - Configured by RRC

3. **Padding BSR:**
   - When MAC PDU has padding
   - Use padding to send BSR
   - 4 types based on padding size

**Implementation:**
```c
if (new_data_arrived || 
    higher_priority_data || 
    periodic_timer_expired ||
    has_padding) {
    trigger_bsr();
}
```"

### **Q4: LCP (Logical Channel Prioritization) kaise kaam karta hai?**

**Answer:**
"LCP UE side mein hota hai jab UL grant milta hai:

**Two Phases:**

**Phase 1: Token Bucket**
Each LC has:
- PBR (Prioritized Bit Rate)
- BSD (Bucket Size Duration)
- Bj (Bucket level)

```c
// Update bucket
Bj += PBR * delta_time;
if (Bj > BSD * PBR) {
    Bj = BSD * PBR;  // Cap
}

// Allocate up to PBR
allocated = min(Bj, available_grant);
```

**Phase 2: Priority Order**
After PBR satisfied, allocate by priority:
- Higher priority LC first
- Until grant exhausted

**Example:**
```
LC1: priority=1, PBR=100
LC2: priority=2, PBR=50

Phase 1: Both get up to PBR
Phase 2: LC1 gets remaining (higher priority)
```"

### **Q5: DRX kya hai?**

**Answer:**
"DRX = Discontinuous Reception
- UE periodically sleeps to save battery
- Wakes up at specific times to check for data

**Parameters:**
- onDurationTimer
- drx-InactivityTimer
- drx-RetransmissionTimer
- shortDRX-Cycle, longDRX-Cycle

**States:**
1. **Active Time:** UE listens
2. **Sleep:** UE saves power

**Trigger Active:**
- Scheduled UL/DL data
- HARQ retransmission
- RAR pending
- Pending SR

Saves up to 50% battery!"

---

## ✅ Self-Test

1. ✅ MAC PDU structure draw karo
2. ✅ HARQ process number kyun 8 hai LTE mein?
3. ✅ Soft combining kaise kaam karta hai?
4. ✅ RACH 4 messages explain karo
5. ✅ Contention-based vs Contention-free difference?
6. ✅ MAC scheduler ke 4 inputs kya hain?
7. ✅ Proportional Fair algorithm ka formula?
8. ✅ BSR trigger conditions?
9. ✅ LCP ke 2 phases?
10. ✅ DRX ka purpose?

---

## 🚀 Next File:
**Read:** `03_RLC_LAYER_DEEP.md`
