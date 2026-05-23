# 🎯 ORAN & NTN - Modern Topics

## 🌐 ORAN (Open Radio Access Network)

### **Why ORAN?**

**Traditional RAN Problems:**
- Vendor lock-in (Huawei, Ericsson, Nokia)
- Closed proprietary interfaces
- Expensive
- Slow innovation

**ORAN Solution:**
- Open interfaces
- Multi-vendor interoperability
- Software-defined
- Innovation-friendly

---

## 📊 ORAN Architecture

### **Functional Split:**

```
Traditional gNB (Monolithic):
┌──────────────────────────┐
│         gNB              │
│  All functions in one    │
└──────────────────────────┘

ORAN Split (CU-DU-RU):
┌──────────────┐
│   gNB-CU     │ Centralized Unit
│ (RRC/PDCP)   │ Layer 3 + PDCP
└──────┬───────┘
       │ F1 Interface
       ↓
┌──────────────┐
│   gNB-DU     │ Distributed Unit
│(RLC/MAC/HIGH-│ Layer 2 + High PHY
│    PHY)      │
└──────┬───────┘
       │ Open Fronthaul (7.2x)
       ↓
┌──────────────┐
│     RU       │ Radio Unit
│ (LOW-PHY/RF) │ Low PHY + RF
└──────────────┘
```

### **Functional Splits:**

**Option 2: PDCP - RLC**
- CU: PDCP, RRC
- DU: RLC, MAC, PHY
- Most common

**Option 6: MAC - PHY**
- CU: All except PHY
- DU: PHY only

**Option 7.2x: HIGH-PHY - LOW-PHY** ⭐ ORAN STANDARD
- DU: MAC + High PHY (some PHY processing)
- RU: Low PHY + RF
- Most flexible

**Option 8: PHY - RF**
- CU/DU: All baseband
- RU: Pure RF

---

## 🎯 ORAN Interfaces

### **F1 Interface (CU ↔ DU):**

**F1-C (Control Plane):**
- F1AP protocol
- UE context management
- Bearer setup
- Mobility

**F1-U (User Plane):**
- GTP-U over UDP/IP
- User data forwarding

### **Open Fronthaul (DU ↔ RU):**

**7.2x Split Specifications:**
- eCPRI protocol
- Lower latency than CPRI
- More flexibility

**Categories:**
- Cat A: Lower complexity RU
- Cat B: More functions in RU

### **E2 Interface (RAN → RIC):**

**RIC = RAN Intelligent Controller**
- Real-time control
- Near-RT RIC: 10ms-1s
- Non-RT RIC: >1s

```
┌────────────┐
│  Non-RT RIC│ AI/ML, policy
└──────┬─────┘
       │
┌──────▼─────┐
│  Near-RT   │ Real-time control
│    RIC     │
└──────┬─────┘
       │ E2 Interface
┌──────▼─────┐
│   gNB      │ Receives commands
└────────────┘
```

---

## 🎯 7.2x Fronthaul

### **Functional Split:**

**DU Side (High PHY):**
- Channel encoding
- Modulation
- Layer mapping
- Resource mapping

**RU Side (Low PHY):**
- Beamforming weights
- iFFT/FFT
- CP insertion/removal
- Digital filtering
- ADC/DAC
- RF

### **Why 7.2x?**

**Pros:**
- Lower fronthaul bandwidth than CPRI
- Multi-vendor interop
- Easier RU upgrade
- Beamforming flexibility

**Cons:**
- More complex than other splits
- Tight latency requirements
- Synchronization critical

### **Categories:**

| Cat | Beamforming | Channel Filter | Function |
|-----|-------------|----------------|----------|
| A | RU | RU | Simpler RU |
| B | DU + RU | RU | Flexible |

---

## 🎯 RIC (RAN Intelligent Controller)

### **Near-RT RIC:**
- Time scale: 10ms - 1s
- Real-time decisions
- Examples:
  - Beam steering
  - QoS adjustment
  - Slice management

### **Non-RT RIC:**
- Time scale: > 1s
- AI/ML training
- Policy management
- Examples:
  - Predictive maintenance
  - Network optimization
  - Anomaly detection

### **xApps (Near-RT RIC):**
- Containerized applications
- Specific use cases
- Pluggable architecture

### **rApps (Non-RT RIC):**
- Long-term optimization
- AI/ML driven
- Policy-based

---

## 🌍 NTN (Non-Terrestrial Networks)

### **What is NTN?**

5G NR via:
- **Satellites** (LEO, MEO, GEO)
- **HAPS** (High Altitude Platform Systems)
- **Aerial Vehicles**

### **Why NTN?**

1. **Coverage:** Remote areas
2. **Resilience:** Disaster recovery
3. **Maritime/Aviation:** Connectivity
4. **IoT:** Global IoT
5. **Backhaul:** For remote cells

---

## 📡 NTN Challenges

### **1. Long Propagation Delay:**

```
LEO (600 km): RTT ~ 4-6 ms
MEO (10000 km): RTT ~ 70 ms
GEO (36000 km): RTT ~ 240 ms
```

### **2. High Doppler:**

```
LEO satellite: 7.5 km/s
Doppler shift: Up to 50 kHz
Doppler rate: Up to 600 Hz/s
```

### **3. Coverage:**
- Wide beams (vs terrestrial)
- Mobility (LEO satellites)

### **4. Synchronization:**
- TA varies during connection
- Need pre-compensation

---

## 🎯 NTN MAC Adaptations

### **1. Extended TA:**

**Terrestrial:**
- Max TA: ~ 100 km
- Cell radius limit

**NTN:**
- Up to 36,000 km (GEO)
- Need MUCH larger TA range

```c
// Terrestrial
TA_max = 1282 × 16 × Ts;  // ~100 km

// NTN
TA_max = NTN_extended_range;  // Up to 240ms (GEO)
```

### **2. Pre-compensation:**

UE pre-compensates:
```c
// Get satellite position
satellite_pos = get_ephemeris_data();

// UE position (from GNSS)
ue_pos = get_gnss_position();

// Calculate distance
distance = calc_distance(ue_pos, satellite_pos);

// Pre-compensate TA
TA = (2 × distance) / c;  // Round trip
```

### **3. Doppler Compensation:**

```c
// Satellite velocity
sat_velocity = get_velocity();

// Relative velocity
rel_velocity = sat_velocity - ue_velocity;

// Doppler shift
doppler = (rel_velocity / c) × frequency;

// Compensate
adjusted_freq = transmitted_freq - doppler;
```

### **4. Long HARQ RTT:**

**Problem:**
- LEO RTT: 4-6 ms (manageable)
- GEO RTT: 240 ms (huge!)
- Standard 8 HARQ insufficient

**Solutions:**
- More HARQ processes (16+ for GEO)
- Disable HARQ (rely on RLC)
- Larger soft buffers

### **5. RACH Adaptations:**

**Extended RAR Window:**
```c
// Terrestrial
ra_response_window = 10ms max;

// NTN GEO
ra_response_window = 280ms;  // RTT + processing
```

**Larger Preamble Sequences:**
- Format A4-C2 with longer CP
- Tolerate larger TA

---

## 🎯 Real Interview Questions

### **Q1: ORAN kya hai aur kyun important hai?**

**Answer:**
"ORAN (Open Radio Access Network) is industry initiative for open, intelligent, multi-vendor RAN.

**Key Goals:**
1. **Open Interfaces:** No vendor lock-in
2. **Multi-vendor:** Mix and match equipment
3. **AI/ML:** Intelligent network
4. **Cloud-native:** Containerized deployment
5. **Cost reduction:** Competition drives down costs

**Components:**
- gNB-CU (Centralized Unit)
- gNB-DU (Distributed Unit)
- RU (Radio Unit)
- RIC (RAN Intelligent Controller)

**Functional Splits:**
- Option 2 (PDCP-RLC)
- Option 7.2x (PHY-PHY) ⭐ ORAN standard

**Mera Experience:**
'O-RAN DU architecture me familiar hoon - High/Low split, 7.2x Fronthaul, DU-RU interface ka knowledge hai.'

**Industry Impact:**
- Major operators adopting (Reliance, Verizon, NTT)
- Promotes innovation
- Reduces costs"

### **Q2: 7.2x split ke baare me batao?**

**Answer:**
"7.2x split ORAN ka standard split hai PHY-PHY:

**Architecture:**
```
DU (High PHY):              RU (Low PHY):
- Channel coding            - Beamforming
- Modulation                - iFFT/FFT
- Layer mapping             - CP add/remove
- Resource mapping          - Digital filter
                            - ADC/DAC, RF
```

**Two Categories:**

**Cat A (Lower RU complexity):**
- Beamforming: RU only
- Channel filter: RU
- Simpler RU
- Less flexibility

**Cat B (More distributed):**
- Beamforming: DU + RU
- Channel filter: RU
- More flexibility
- Complex DU

**Protocol: eCPRI (enhanced CPRI)**
- Ethernet-based
- Lower BW than CPRI
- Multi-vendor interop
- Latency-sensitive

**Latency Requirements:**
- T1: 50-100 μs
- Need precision time sync
- IEEE 1588 PTP

**Why important?**
- Multi-vendor RAN
- Innovation friendly
- Cost reduction
- Beamforming flexibility"

### **Q3: NTN kya hai aur challenges?**

**Answer:**
"NTN (Non-Terrestrial Networks) 5G NR via satellites/HAPS:

**Types:**

| Type | Altitude | RTT | Use Case |
|------|----------|-----|----------|
| LEO | 600-2000 km | 4-15 ms | High BW, low latency |
| MEO | 8000-20000 km | 50-150 ms | Medium |
| GEO | 36000 km | 240 ms | Wide coverage |
| HAPS | 20 km | 130 μs | Local |

**Major Challenges:**

**1. Long RTT:**
- Standard HARQ (8 procs) insufficient for GEO
- Need 16+ processes or disable HARQ

**2. High Doppler:**
- LEO: 7.5 km/s velocity
- 50+ kHz frequency shift
- Need compensation

**3. Coverage:**
- Wide beams (1000+ km diameter)
- Mobility for LEO
- Beam handover frequent

**4. Synchronization:**
- TA varies dynamically
- Need GNSS at UE
- Pre-compensation

**5. RACH:**
- Long RAR windows (280ms for GEO)
- Larger TA range
- Special preamble formats

**MAC Adaptations:**
- Extended TA support
- Doppler shift handling
- More HARQ processes
- Adapted RACH timers

**Mera Knowledge:**
'NTN MAC adaptations - extended TA, Doppler compensation - conceptually familiar hoon.'

**Real Operators:**
- Starlink (SpaceX)
- OneWeb
- Amazon Kuiper
- 3GPP Rel-17 NTN support"

### **Q4: ORAN me xApp kya hai?**

**Answer:**
"xApps Near-RT RIC ke applications hain:

**Definition:**
- Containerized applications
- Run on Near-RT RIC platform
- Specific use cases
- Real-time control

**Lifecycle:**
1. Onboard
2. Deploy
3. Configure
4. Monitor
5. Update/Remove

**Examples:**

**Beam Management xApp:**
```c
on_event_received(beam_failure_event) {
    // Analyze beam quality
    new_beam = select_best_beam();
    
    // Send command via E2
    send_e2_control(new_beam_config);
}
```

**QoS Optimization xApp:**
```c
on_kpi_threshold_exceeded() {
    // Adjust scheduling priority
    new_qos = compute_optimal_qos();
    update_scheduler_policy(new_qos);
}
```

**Slice Management xApp:**
```c
on_slice_demand_change() {
    // Reallocate resources
    rebalance_resources_per_slice();
}
```

**E2 Interface:**
- xApp ↔ gNB communication
- Service models (KPM, RC, etc.)
- Real-time decisions

**Benefits:**
- Modular development
- Quick deployment
- Open ecosystem
- AI/ML integration"

### **Q5: NTN me HARQ kaise handle karte hain?**

**Answer:**
"NTN me HARQ challenging hai due to long RTT:

**Problem:**
```
LEO HARQ RTT: 8-12 ms
- 8 processes work
- Marginal performance

GEO HARQ RTT: 480 ms
- 8 processes utilization < 1%
- Massive throughput drop
```

**Solutions:**

**1. More HARQ Processes:**
```c
// Standard: 8
HARQ_NUM_PROCESSES = 8;

// NTN GEO:
HARQ_NUM_PROCESSES = 32;  // Or even disable
```

**2. Disable HARQ for GEO:**
```c
if (link_type == NTN_GEO) {
    harq_disabled = true;
    // Rely on RLC AM ARQ instead
    // Slower but works
}
```

**3. Number of Processes Configuration:**
- 3GPP Rel-17 NTN
- Configurable up to 32
- Per-cell config

**4. Skip Feedback:**
- Some NTN modes skip ACK/NACK
- Better throughput
- Higher BLER acceptable

**5. Larger Soft Buffer:**
- More processes need more memory
- Per-process soft buffer
- Memory cost

**6. Timing Adaptations:**
```c
// Adaptive HARQ feedback timing
if (ntn_mode) {
    feedback_timing = ntn_specific_timing;
} else {
    feedback_timing = 4;  // standard
}
```

**Trade-offs:**
- More processes = more memory
- No HARQ = rely on RLC (slower)
- Skip feedback = lower reliability

**My approach:**
'NTN MAC adaptations conceptually familiar hoon. Practical implementation me extended TA, Doppler, aur HARQ adjustments key challenges hain.'"

### **Q6: ORAN aur traditional RAN me difference?**

**Answer:**
"
| Feature | Traditional | ORAN |
|---------|-------------|------|
| Vendor | Single | Multi |
| Interfaces | Proprietary | Open |
| Software | Closed | Open source friendly |
| Innovation | Slow | Fast |
| Cost | High | Lower |
| AI/ML | Limited | Native (RIC) |
| Cloud | Difficult | Cloud-native |
| Customization | Vendor dependent | Operator control |

**Architecture Difference:**

**Traditional:**
```
   gNB (Monolithic)
   - Vendor X only
   - All functions
   - Proprietary
```

**ORAN:**
```
gNB-CU (Vendor A)
    ↓ F1 (open)
gNB-DU (Vendor B)
    ↓ Fronthaul (open)
RU (Vendor C)
    ↑ E2 (open)
RIC (Vendor D + xApps)
```

**Adoption:**
- Reliance Jio: Full ORAN
- Verizon: ORAN trial
- AT&T: Mixed
- Most major operators planning"

---

## 📊 Quick Comparison: ORAN Splits

| Split | Where | Latency | Bandwidth |
|-------|-------|---------|-----------|
| Option 1 | RRC-PDCP | High | Low |
| Option 2 | PDCP-RLC | Medium | Medium |
| Option 6 | MAC-PHY | Low | High |
| 7.2x | PHY-PHY ⭐ | Very Low | High |
| Option 8 | PHY-RF | Lowest | Highest |

---

## ✅ Self-Test

1. ✅ ORAN kya hai aur kyun?
2. ✅ CU, DU, RU functions?
3. ✅ Functional splits (Option 2, 7.2x)?
4. ✅ F1, E2 interfaces?
5. ✅ Near-RT vs Non-RT RIC?
6. ✅ NTN types (LEO, MEO, GEO)?
7. ✅ NTN MAC challenges?
8. ✅ Doppler compensation?
9. ✅ Extended TA in NTN?
10. ✅ HARQ in NTN?

---

## 🚀 Next File:
**Read:** `09_CPP_INTERVIEW.md`
