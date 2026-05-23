# 🎯 RACH & HARQ - Tumhari EXPERTISE! 🌟

## 🎯 RACH (Random Access Channel)

### **Why RACH?**
- UE ko initial network access
- Synchronize timing with eNB/gNB
- Get UL grant for first transmission
- Re-establish after RLF

### **When RACH triggered?**

1. **Initial Access** - Power on, network entry
2. **RRC Connection Re-establishment** - After RLF
3. **Handover** - To target cell
4. **DL data arrival** - When UE out-of-sync
5. **UL data arrival** - When UE out-of-sync
6. **Beam failure recovery** (5G NR)
7. **System info request** (5G NR)
8. **Positioning** (5G NR)

---

## 🎯 RACH Procedure - 4-Step (CBRA)

### **Detailed Flow:**

```
┌────┐                         ┌────────┐
│ UE │                         │eNB/gNB │
└─┬──┘                         └────┬───┘
  │                                 │
  │ Read SIB2 (RACH config)         │
  │                                 │
  │ ━━━ Msg1: Preamble ━━━          │
  │   - Random preamble (1 of 64)   │
  │   - On PRACH                    │
  │   - Power: PREAMBLE_INIT_POWER  │
  ├────────────────────────────────→│
  │                                 │
  │ Wait for RAR window             │ Process preamble
  │ (3-10 ms)                       │ Estimate TA
  │                                 │
  │ ━━━ Msg2: RAR ━━━                │
  │   - RA-RNTI                     │
  │   - RAPID                       │
  │   - TA command                  │
  │   - UL grant for Msg3           │
  │   - Temporary C-RNTI            │
  │←────────────────────────────────│
  │                                 │
  │ Apply TA, prepare Msg3          │
  │                                 │
  │ ━━━ Msg3: Scheduled TX ━━━      │
  │   - RRC Connection Request      │
  │     OR RRC Re-establishment     │
  │     OR C-RNTI MAC CE            │
  │   - HARQ enabled                │
  │   - Scrambled with TC-RNTI      │
  ├────────────────────────────────→│
  │                                 │
  │                                 │ Resolve contention
  │                                 │
  │ ━━━ Msg4: Contention Resolution│
  │   - Contention Resolution ID    │
  │   - Or RRC Connection Setup     │
  │   - PDSCH                       │
  │←────────────────────────────────│
  │                                 │
  │ Compare ID, if match → success  │
  │ If no match → fail, retry RACH  │
  │                                 │
  ▼                                 ▼
```

---

## 🎯 Msg1 - Preamble Transmission

### **Preamble Structure:**

**Format 0-3 (LTE):**
- Format 0: Normal cell (cycle 800 μs)
- Format 1: Large cell (1600 μs)
- Format 2: Long preamble (1600 μs)
- Format 3: Very long (large cell)

**Format A0-C2 (5G NR):**
- Long: A0-A3, B1-B4
- Short: C0, C2 (high frequency)

### **Zadoff-Chu Sequence:**

```
Mathematical sequence with:
- Constant amplitude
- Zero auto-correlation
- Low cross-correlation
- Good detection properties

64 preambles per cell
Cyclic shifts for orthogonality
```

### **Preamble Selection:**

**Group A vs Group B:**
- Group A: For small payload (Msg3)
- Group B: For large payload

**Selection Logic:**
```c
uint8_t select_preamble(int msg3_size) {
    int threshold = SIB2.preamble_payload_threshold;
    
    if (msg3_size <= threshold || group_b_unavailable) {
        // Use Group A
        return random(group_a_preambles);
    } else {
        // Use Group B
        return random(group_b_preambles);
    }
}
```

### **CBRA vs CFRA Preamble:**

**CBRA (Contention-Based):**
- Random preamble from group
- May collision with other UEs
- Need contention resolution

**CFRA (Contention-Free):**
- Dedicated preamble (assigned by RRC)
- No collision possible
- For HO, beam recovery

### **Power Control:**

```c
preamble_init_power = preambleInitialReceivedTargetPower + DELTA_PREAMBLE;

// Power ramping
for each retry:
    if (no RAR received) {
        preamble_power += POWER_RAMP_STEP;  // e.g., 2 dB
        preamble_tx_counter++;
    }

// Limits
preamble_power <= UE_MAX_POWER;
preamble_tx_counter <= preambleTransMax;  // e.g., 200
```

---

## 🎯 Msg2 - RAR (Random Access Response)

### **RAR Window:**

```c
ra_response_window_start = preamble_tx_subframe + 3;
ra_response_window_end = ra_response_window_start + ra_response_window_size;

// Window size: 2-10 subframes
// UE monitors PDCCH with RA-RNTI in this window
```

### **RA-RNTI Calculation:**

**LTE:**
```c
RA-RNTI = 1 + t_id + 10 × f_id;
// t_id: subframe of preamble (0-9)
// f_id: PRACH freq resource (0-5)
```

**5G NR:**
```c
RA-RNTI = 1 + s_id + 14 × t_id + 14 × 80 × f_id + 14 × 80 × 8 × ul_carrier_id;
// More complex due to flexibility
```

### **RAR Contents:**

```
┌──────────┬──────────┬──────────────────┐
│ RAPID    │ TA Cmd   │ UL Grant +       │
│          │ (11 bits)│ TC-RNTI          │
└──────────┴──────────┴──────────────────┘

RAPID: Random Access Preamble ID (which preamble matched)
TA: Timing Advance command (0-1282 in 16Ts steps)
UL Grant: 20 bits for Msg3 scheduling
TC-RNTI: 16-bit Temporary C-RNTI
```

### **TA (Timing Advance):**

```c
// TA in time units
TA_seconds = TA_value × 16 × Ts;
Ts = 1 / (15000 × 2048);  // LTE basic unit

// Distance from cell
distance = (TA_seconds × c) / 2;  // c = speed of light

// Round-trip time
RTT = 2 × distance / c;
```

**Why TA?**
- Different UEs different distances
- Without TA: UL transmissions don't align
- TA: UE transmits earlier to compensate

### **Backoff:**

```c
// If preamble Group A or B unavailable
if (rar_indicates_backoff) {
    backoff_time = random(0, BI_value);
    // BI: 0=0ms, 1=10ms, 2=20ms, ..., 12=960ms
    
    wait(backoff_time);
}
```

---

## 🎯 Msg3 - Scheduled Transmission

### **What's in Msg3?**

**For Initial Access:**
```
RRC Connection Request:
- UE Identity (S-TMSI or random)
- Establishment Cause
```

**For Re-establishment:**
```
RRC Connection Re-establishment Request:
- C-RNTI (old)
- physCellID
- shortMAC-I (security)
- Re-establishment Cause
```

**For Handover:**
```
C-RNTI MAC CE
- Old C-RNTI (16 bits)
- Identifies UE in target cell
```

### **Msg3 HARQ:**

- HARQ enabled
- Up to 4-5 retransmissions
- Based on `maxHARQ-Msg3Tx`

```c
void msg3_transmit() {
    int harq_attempts = 0;
    int max_attempts = SIB2.maxHARQ_Msg3Tx;
    
    do {
        send_msg3();
        wait_for_ack_nack();
        
        if (ack_received) break;
        if (++harq_attempts >= max_attempts) {
            handle_msg3_failure();  // Restart RACH
            break;
        }
    } while (1);
}
```

### **Power for Msg3:**

```c
P_Msg3 = min(P_max, 
             P_O_PUSCH + 
             10×log10(M_PUSCH) + 
             alpha × PL +
             delta_TF +
             f(i));
```

---

## 🎯 Msg4 - Contention Resolution

### **Contention Resolution ID MAC CE:**

```
For Initial Access:
  CR-ID = First 48 bits of CCCH SDU from Msg3
  
For Re-establishment:
  CR-ID = old C-RNTI + identity-related info
  
For Handover (CFRA):
  No contention, just RRC messages
```

### **UE Side:**

```c
void on_msg4_received(msg4_t *msg) {
    if (msg->is_dci_with_tc_rnti) {
        // Decoded with TC-RNTI
        if (msg->cr_id == ue->cr_id) {
            // Match! UE wins contention
            ue->c_rnti = ue->tc_rnti;
            rach_success();
        } else {
            // No match, UE loses
            rach_failure();
            restart_rach();
        }
    }
}
```

### **Contention Resolution Timer:**

```c
// Started after Msg3
start_timer(mac_contention_resolution_timer);
// Default: 8-64 subframes

if (timer_expired_without_msg4) {
    // Failure
    increment_preamble_tx_counter();
    if (counter > max_attempts) {
        report_to_rrc_failure();
    } else {
        retry_rach();
    }
}
```

---

## 🎯 5G NR 2-Step RACH

### **Why 2-Step?**
- Lower latency
- Reduced signaling overhead
- Better for IoT, URLLC

### **MsgA (Combines Msg1 + Msg3):**

```
MsgA = Preamble + Payload

Payload includes:
- RRC message
- Or buffer info
- Or small data
```

### **MsgB (Combines Msg2 + Msg4):**

```
MsgB:
- Success: Like Msg2 + Msg4
- Fallback: Like Msg2 only (fall back to 4-step)
```

### **Implementation:**

```c
typedef enum {
    RACH_2STEP,
    RACH_4STEP
} rach_type_t;

void rach_2step_procedure() {
    // Build MsgA
    msga_t msga = {
        .preamble = select_preamble(),
        .payload = build_payload()
    };
    
    transmit_msga(msga);
    
    // Wait for MsgB
    msgb_t msgb = wait_for_msgb();
    
    if (msgb.is_success) {
        // 2-step succeeded
        handle_success(msgb);
    } else if (msgb.is_fallback) {
        // Fall back to 4-step
        send_msg3();
        wait_for_msg4();
    } else {
        // Failure, retry
        retry_rach();
    }
}
```

---

## 🎯 HARQ Deep Dive (Tumhari Expertise!)

### **HARQ Process:**

```c
typedef struct {
    uint8_t process_id;       // 0-7 (LTE) or 0-15 (NR)
    bool active;
    bool pending_retx;
    uint8_t rv;               // Redundancy version (0,1,2,3)
    uint8_t ndi;              // New Data Indicator
    uint8_t retx_count;
    uint32_t tb_size;
    uint8_t *tb_buffer;       // Transport block
    uint8_t *soft_buffer;     // Soft combining buffer
    timer_t harq_timer;
} harq_process_t;

typedef struct {
    harq_process_t processes[MAX_HARQ_PROCESSES];
    uint8_t active_mask;       // Bitmap
} harq_entity_t;
```

### **8 Stop-and-Wait:**

```
Time: 1ms 2ms 3ms 4ms 5ms 6ms 7ms 8ms 9ms 10ms
P0:   TX  --  --  --  --  --  --  --  ACK
P1:   --  TX  --  --  --  --  --  --   --  ACK
P2:   --  --  TX  --  --  --  --  --   --   --  ACK
P3:   --  --  --  TX  --  --  --  --   --   --   --  ACK
P4:   --  --  --  --  TX  --  --  --   --   --   --
P5:   --  --  --  --  --  TX  --  --   --   --
P6:   --  --  --  --  --  --  TX  --   --
P7:   --  --  --  --  --  --  --  TX

After 8ms, P0 gets ACK, can be reused
```

### **Redundancy Versions (RV):**

```
RV0: Systematic bits (mostly data)
RV1: Parity bits (subset)
RV2: More parity bits
RV3: Most redundancy
```

**Order:** RV0 → RV2 → RV3 → RV1 (typical)

### **Soft Combining:**

**Chase Combining (CC):**
```
Each retx: Same data
Soft combine: Add LLRs (log-likelihood ratios)
    LLR_combined = LLR_1 + LLR_2 + LLR_3 + ...
```

**Incremental Redundancy (IR):**
```
Each retx: Different RV (more parity)
Better than CC, used in LTE/5G
```

### **HARQ in 5G NR:**

**Differences from LTE:**
1. Up to 16 HARQ processes (vs 8)
2. All async (LTE: DL sync)
3. Code Block Group (CBG) feedback
4. Mini-slot HARQ

**CBG (Code Block Group):**
```
Large TB → Multiple code blocks → CBGs

Per-CBG ACK/NACK:
- Retransmit only failed CBGs
- More efficient than per-TB
```

---

## 🎯 Real Interview Questions

### **Q1: RACH ke 4 messages detail mein explain karo?**

**Answer:** *(Tumhare expertise wala!)*

"RACH 4-step procedure mera daily kaam hai. Let me explain:

**Msg1 - Preamble Transmission:**
- UE selects 1 of 64 preambles (Zadoff-Chu sequence)
- Group A (small payload) or Group B (large payload)
- Transmits on PRACH with initial power
- Specific time-frequency resource (per RACH config)

**Msg2 - RAR (Random Access Response):**
Within RAR window (2-10 ms):
- eNB detects preamble
- Estimates TA (Timing Advance)
- Sends RAR on PDSCH (scrambled with RA-RNTI)
- Contents: RAPID, TA cmd, UL grant for Msg3, TC-RNTI

**Msg3 - Scheduled Transmission:**
- For initial: RRC Connection Request
- For re-est: RRC Re-establishment Request
- For HO: C-RNTI MAC CE
- HARQ enabled (up to 4-5 retx)
- Apply TA from Msg2

**Msg4 - Contention Resolution:**
- For CBRA: Contention Resolution Identity MAC CE
- UE compares with its own ID
- Match → win contention
- No match → restart RACH
- TC-RNTI promoted to C-RNTI

**Mera 5G NR RACH Simulator** is feature complete - I implemented all these states with timing validation per 3GPP TS 38.321."

### **Q2: HARQ aur ARQ mein kya difference hai?**

**Answer:**
"HARQ (MAC) aur ARQ (RLC) dono retransmission karte hain but different layers pe:

| Feature | HARQ (MAC) | ARQ (RLC) |
|---------|-----------|-----------|
| Layer | MAC | RLC |
| Speed | Fast (~8ms) | Slow (~100ms+) |
| Mechanism | Soft combining | Plain retx |
| FEC | Yes | No |
| Window | 8 procs | Larger |
| Detection | CRC | SN-based |

**Why both?**
- HARQ catches most errors (MAC)
- ARQ catches missed by HARQ (RLC AM)
- Two-layer reliability

**Implementation in my work:**
```c
// HARQ at MAC
if (crc_failed) {
    request_retransmission_via_harq();
    soft_combine_with_previous();
}

// ARQ at RLC (after MAC HARQ fails)
if (harq_max_retx_exceeded) {
    notify_rlc();
    rlc_am_retransmit();
}
```"

### **Q3: HARQ process kyun 8 hai LTE mein?**

**Answer:**
"LTE FDD me 8 HARQ processes hain due to timing:

**HARQ RTT (Round Trip Time):**
- 1 TTI for transmission (1 ms)
- 3 TTI for processing
- 1 TTI for ACK/NACK transmission
- 3 TTI for processing
- Total: 8 TTI = 8 ms

**Why 8 processes?**
- While process 1 waits 8ms for ACK
- Process 2-8 transmit
- 100% utilization
- Pipeline architecture

**TDD different:**
- Different DL/UL config
- Variable processes (4, 6, 7, 10, 11, 12, 15)

**5G NR:**
- Up to 16 processes
- Async DL (more flexibility)
- Mini-slot HARQ"

### **Q4: Soft combining kaise kaam karta hai?**

**Answer:**
"Soft combining HARQ ka key feature hai:

**Concept:**
- Receiver stores soft bits (LLRs) of failed transmission
- On retx, combine soft bits with new transmission
- Decode combined data
- Higher SNR effectively

**Chase Combining (CC):**
```
TX1: Same data
TX2: Same data
Combined LLR = LLR1 + LLR2
Effective SNR ~ 2x (3 dB gain)
```

**Incremental Redundancy (IR):**
```
TX1: RV0 (systematic + some parity)
TX2: RV2 (more parity)
TX3: RV3 (most parity)
Combined: Different code rate, better FEC
```

**Implementation:**
```c
typedef struct {
    int8_t *soft_bits;  // Signed LLRs
    int size;
    int rv;
} soft_buffer_t;

void store_soft_bits(harq_process_t *proc, int8_t *llrs, int size) {
    if (proc->rv == 0) {
        // First transmission, store
        memcpy(proc->soft_buffer.soft_bits, llrs, size);
    } else {
        // Retransmission, combine
        for (int i = 0; i < size; i++) {
            proc->soft_buffer.soft_bits[i] += llrs[i];
            // Saturate to prevent overflow
            proc->soft_buffer.soft_bits[i] = 
                clip(proc->soft_buffer.soft_bits[i], -127, 127);
        }
    }
}
```"

### **Q5: 5G NR RACH ke baare me batao?**

**Answer:**
"5G NR RACH improvements:

**1. 2-Step RACH:**
- MsgA = Preamble + Payload
- MsgB = RAR + CR
- Lower latency (~50% reduction)
- Good for URLLC, IoT

**2. Beam-based RACH:**
- mmWave needs beamforming
- Multiple SSBs (sync signals)
- UE selects best beam preamble
- TA per beam

**3. Larger Preambles:**
- Up to 64 preambles per beam
- More than LTE's 64 total
- Better with multiple beams

**4. Scaled Numerology:**
- 1.25 kHz, 2.5 kHz, 5 kHz SCS
- Format A0-C2
- Flexible RACH timing

**5. Restricted Set Type B:**
- For high-speed UEs
- Up to 1000 km/h support

**6. 4-Step Fallback:**
- 2-step can fall back to 4-step
- Robust handling

**7. SUL (Supplementary UL):**
- Lower freq UL for coverage
- Better RACH success

**Mera RACH Simulator** covers both 4-step and 2-step procedures with these enhancements."

### **Q6: Apna 5G NR RACH Simulator project explain karo?**

**Answer:** *(STAR project!)*

"Mera **5G NR MAC RACH Process Simulator** key project hai:

**Architecture:**
```
┌──────────────────────────────────┐
│      5G NR RACH Simulator        │
├──────────────────────────────────┤
│ State Machine                    │
│  - INIT → PREAMBLE → RAR_WAIT   │
│  - → MSG3 → MSG4 → SUCCESS      │
├──────────────────────────────────┤
│ Configuration                    │
│  - Per 3GPP TS 38.321           │
│  - Configurable parameters      │
├──────────────────────────────────┤
│ Timing                           │
│  - Subframe-level granularity   │
│  - RAR window, CR timer         │
├──────────────────────────────────┤
│ Validation                       │
│  - 3GPP spec compliance         │
│  - Edge cases handled           │
└──────────────────────────────────┘
```

**Implementation Details:**

**1. State Machine:**
```c
typedef enum {
    RACH_IDLE,
    RACH_PREAMBLE_TX,
    RACH_WAIT_RAR,
    RACH_RAR_RECEIVED,
    RACH_MSG3_TX,
    RACH_WAIT_MSG4,
    RACH_SUCCESS,
    RACH_FAILED
} rach_state_t;

void rach_state_machine(rach_event_t event) {
    switch(current_state) {
        case RACH_IDLE:
            if (event == TRIGGER) {
                send_preamble();
                transition_to(RACH_PREAMBLE_TX);
            }
            break;
        case RACH_PREAMBLE_TX:
            transition_to(RACH_WAIT_RAR);
            start_rar_window();
            break;
        // ... more states
    }
}
```

**2. Both Modes:**
- CBRA (Contention-Based)
- CFRA (Contention-Free)
- 4-step and 2-step (NR)

**3. Validation:**
- All timing windows tested
- Backoff scenarios
- Power ramping
- Max attempts handling

**4. Results:**
- Pass all 3GPP test cases
- Used as reference implementation
- Helped train new team members"

### **Q7: RACH failure handling kaise hota hai?**

**Answer:**
"Multiple failure scenarios:

**1. No RAR received:**
```c
if (rar_window_expired && !rar_received) {
    preamble_tx_counter++;
    if (preamble_tx_counter > preambleTransMax) {
        // Report to RRC
        rrc_indicate_radio_link_failure();
    } else {
        // Power ramp up
        preamble_power += POWER_RAMP_STEP;
        // Backoff
        wait(random_backoff());
        retry_rach();
    }
}
```

**2. Msg3 HARQ failure:**
```c
if (msg3_harq_attempts > maxHARQ_Msg3Tx) {
    // Restart RACH from Msg1
    increment_preamble_counter();
    retry_rach();
}
```

**3. No Msg4 received:**
```c
if (contention_resolution_timer_expired) {
    // Lost contention
    increment_preamble_counter();
    retry_rach();
}
```

**4. Wrong Contention Resolution:**
```c
if (msg4_cr_id != my_cr_id) {
    // Different UE won
    increment_preamble_counter();
    retry_rach();
}
```

**Final failure:**
- Report Radio Link Failure (RLF)
- Trigger RRC Connection Re-establishment
- Or full IDLE → RRC connection setup"

### **Q8: HARQ NDI (New Data Indicator) kya hai?**

**Answer:**
"NDI 1-bit field jo new data vs retransmission indicate karta hai:

**Concept:**
```
NDI bit:
- Toggled = New data
- Same as last = Retransmission
```

**Why?**
- Disambiguity between new and retx
- Same HARQ process can have new or retx
- UE knows whether to combine or replace

**Sender (eNB):**
```c
if (new_data) {
    proc->ndi = !proc->ndi;  // Toggle
    proc->rv = 0;             // Start with RV0
} else {
    // Same NDI, increment RV
    proc->rv = next_rv(proc->rv);  // 0→2→3→1
}
```

**Receiver (UE):**
```c
if (received_ndi != stored_ndi) {
    // New data
    clear_soft_buffer(harq_process);
    store_new_data();
    stored_ndi = received_ndi;
} else {
    // Retransmission
    combine_with_soft_buffer();
}
```

**5G NR:**
- Same concept
- 1-bit NDI in DCI
- Critical for HARQ correctness"

---

## ✅ Self-Test (TUMHARI EXPERTISE!)

1. ✅ RACH 4-step procedure step-by-step?
2. ✅ Msg1 me preamble selection logic?
3. ✅ RA-RNTI calculation formula?
4. ✅ TA (Timing Advance) ka purpose?
5. ✅ Msg3 me kya bhejte hain?
6. ✅ Contention Resolution kaise hoti hai?
7. ✅ 5G NR 2-step RACH advantages?
8. ✅ 8 HARQ processes kyun?
9. ✅ Soft combining (CC vs IR)?
10. ✅ NDI ka use kya hai?

---

## 🚀 Next File:
**Read:** `08_ORAN_NTN.md`
