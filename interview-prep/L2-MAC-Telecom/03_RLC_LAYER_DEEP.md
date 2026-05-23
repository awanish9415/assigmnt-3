# 🎯 RLC Layer - Complete Deep Dive

## 🎯 RLC Overview

**RLC = Radio Link Control**
- 3GPP TS 36.322 (LTE) / TS 38.322 (5G NR)
- Layer 2 sub-layer (between PDCP and MAC)
- Provides reliable data transfer

### **Three Modes:**

| Mode | Full Form | Use Case | ARQ |
|------|-----------|----------|-----|
| **TM** | Transparent Mode | BCCH, PCCH | No |
| **UM** | Unacknowledged Mode | VoLTE, streaming | No |
| **AM** | Acknowledged Mode | Web, file transfer | Yes ✅ |

---

## 1️⃣ TM (Transparent Mode)

### **Characteristics:**
- No RLC header
- No segmentation
- No reassembly
- No ARQ
- Pass-through mode

### **Usage:**
- Broadcast Channel (BCCH)
- Paging Channel (PCCH)
- CCCH (Common Control Channel)

### **Why Transparent?**
- Control messages already small
- Need fast delivery
- No reliability needed

```
PDCP SDU → RLC TM → MAC SDU
(No modification, just forwarding)
```

---

## 2️⃣ UM (Unacknowledged Mode)

### **Characteristics:**
- Has RLC header (with SN)
- Segmentation (if needed)
- Reassembly at receiver
- **No ARQ** (no retransmission)
- In-order delivery (within timer)

### **Usage:**
- VoIP/VoLTE (low latency needed)
- Video streaming
- Real-time data

### **UM PDU Format:**

```
┌────┬──────┬──────┬─────────┐
│ SI │  R   │  SN  │  Data   │
└────┴──────┴──────┴─────────┘
SI = Segmentation Info
SN = Sequence Number (6 or 12 bits in NR)
```

### **Q: UM mein retransmission kyun nahi?**

**Answer:**
"UM real-time applications ke liye hai jaise voice. Voice mein:
- 200ms se zyada delay = bad quality
- Lost packet retransmit karne ka time nahi
- Better to drop than delay
- Codec handles small losses

That's why UM mein no ARQ - latency over reliability."

---

## 3️⃣ AM (Acknowledged Mode) ⭐ MOST IMPORTANT

### **Characteristics:**
- Reliable delivery (ARQ)
- Sequence numbering
- Segmentation/Reassembly
- Status reports
- Polling
- Window-based flow control

### **Usage:**
- TCP/IP data
- File transfer
- Web browsing
- Email
- Most user data

### **AM PDU Types:**

**1. AMD PDU (AM Data PDU):**
```
┌────┬──────┬──────┬──────┬─────────┐
│D/C │ P/SI │  R   │  SN  │  Data   │
└────┴──────┴──────┴──────┴─────────┘
D/C = Data/Control bit (1 = Data)
P = Polling bit
SI = Segmentation Info
SN = 12 or 18 bit Sequence Number
```

**2. STATUS PDU (Control PDU):**
```
┌────┬─────┬──────┬──────┬──────┐
│D/C │CPT  │ ACK_ │NACK_ │ E1   │
│    │     │  SN  │  SN  │      │
└────┴─────┴──────┴──────┴──────┘

D/C = 0 (Control)
CPT = Control PDU Type
ACK_SN = Highest received SN + 1
NACK_SN = SNs not received
```

---

## 🔄 ARQ Mechanism in AM

### **Sender Side:**

```c
// Send window
struct rlc_am_send {
    uint32_t TX_Next;      // Next SN to use
    uint32_t TX_Next_Ack;  // Highest acked
    uint32_t window_size;
    rlc_pdu_t *buffer;     // Outstanding PDUs
    timer_t poll_timer;
    timer_t reassembly_timer;
};
```

### **Receiver Side:**

```c
// Receive window
struct rlc_am_recv {
    uint32_t RX_Next;          // Next expected SN
    uint32_t RX_Next_Status;   // Status report
    uint32_t RX_Highest_Status;
    rlc_pdu_t *buffer;         // Out-of-order PDUs
    timer_t status_prohibit;
    timer_t reassembly;
};
```

### **ARQ Flow:**

```
TX                          RX
│                            │
│ Send PDU(SN=1) [P=0]       │
├───────────────────────────→│
│ Send PDU(SN=2) [P=0]       │
├───────────────────────────→│ ❌ Lost
│ Send PDU(SN=3) [P=1]       │ Poll
├───────────────────────────→│
│                            │ Detect missing SN=2
│   STATUS PDU               │
│   ACK_SN=4, NACK=2         │
│←───────────────────────────│
│                            │
│ Retransmit PDU(SN=2)       │
├───────────────────────────→│
│                            │
│ All PDUs received ✓        │
```

### **Polling Mechanism:**

When to poll?
1. Every X PDUs (PollPDU)
2. Every X bytes (PollByte)
3. Last PDU in buffer
4. Window almost full

```c
if (pdu_count_since_poll >= PollPDU ||
    bytes_since_poll >= PollByte ||
    is_last_pdu ||
    window_almost_full) {
    set_polling_bit();
    start_t_PollRetransmit();
}
```

---

## 🔧 RLC Functions Detail

### **1. Segmentation:**

When PDCP PDU > MAC grant:

```
PDCP PDU (1500 bytes)
        ↓
   ┌────┴────┐
   ↓         ↓
RLC SDU 1  RLC SDU 2
(800 B)    (700 B)
```

**Header SI field:**
- 00: Complete
- 01: First segment
- 11: Middle segment
- 10: Last segment

### **2. Reassembly:**

Receiver collects segments:

```c
void rlc_am_reassemble(rlc_pdu_t *pdu) {
    if (pdu->si == FIRST_SEGMENT) {
        start_reassembly(pdu->sn);
    } else if (pdu->si == LAST_SEGMENT) {
        complete_reassembly(pdu->sn);
        deliver_to_pdcp();
    } else {
        store_segment(pdu);
    }
}
```

### **3. Duplicate Detection:**

```c
if (received_SN < RX_Next || 
    sn_already_received(received_SN)) {
    discard_pdu();
    return;
}
```

### **4. In-Order Delivery:**

Buffer out-of-order PDUs:

```c
void deliver_in_order() {
    while (buffer[RX_Next].complete) {
        deliver_to_pdcp(buffer[RX_Next]);
        RX_Next++;
    }
}
```

---

## 🎯 RLC Timers

### **Important Timers:**

| Timer | Purpose | Default |
|-------|---------|---------|
| **t-PollRetransmit** | Wait for status after poll | 50-200 ms |
| **t-Reassembly** | Wait for missing segments | 35-200 ms |
| **t-StatusProhibit** | Min interval between status | 10-100 ms |

### **t-Reassembly:**

```c
// Start when out-of-order PDU received
if (received_pdu.sn > RX_Next) {
    if (!reassembly_running) {
        start_t_reassembly();
    }
}

// Expired - missing PDUs declared lost
on_t_reassembly_expiry() {
    update_RX_Highest_Status();
    trigger_status_report();
}
```

### **t-PollRetransmit:**

```c
// Started when poll sent
if (poll_sent) {
    start_t_PollRetransmit();
}

// Expired - retransmit oldest PDU
on_t_PollRetransmit_expiry() {
    if (no_status_received) {
        retransmit_oldest_pdu();
    }
}
```

---

## 🎯 Real Interview Questions

### **Q1: RLC AM aur UM mein difference?**

**Answer:**
"RLC AM aur UM mein major differences:

| Feature | AM | UM |
|---------|----|----|
| Reliability | Reliable (ARQ) | Best effort |
| ARQ | Yes | No |
| Latency | Higher | Lower |
| Use case | Data (TCP) | Voice (VoLTE) |
| Complexity | Complex | Simpler |
| Sequence numbers | 12/18 bits | 6/12 bits |

AM mein retransmission hota hai NACK pe, UM mein simply drop kar dete hain. Real-time apps UM use karte hain kyunki delayed data useless hota hai."

### **Q2: RLC mein ARQ kab use karte ho?**

**Answer:**
"RLC ARQ AM mode mein use hota hai. Procedure:

**Sender:**
1. Maintain transmit buffer with SNs
2. Set polling bit periodically
3. Start t-PollRetransmit
4. On NACK in status PDU → retransmit
5. On ACK → remove from buffer

**Receiver:**
1. Track received SNs
2. Detect missing SNs
3. Start t-Reassembly for out-of-order
4. Send STATUS PDU with NACK list
5. Use t-StatusProhibit to avoid flooding

**Implementation:**
```c
typedef struct {
    uint32_t sn;
    uint8_t *data;
    uint32_t length;
    bool acked;
    uint8_t retx_count;
    timer_t timer;
} rlc_am_pdu_t;
```"

### **Q3: Polling mechanism explain karo?**

**Answer:**
"Polling RLC AM mein receiver ko status report ke liye trigger karta hai:

**Trigger Conditions:**
1. Every PollPDU (e.g., 64 PDUs)
2. Every PollByte (e.g., 25KB)
3. Last PDU in buffer
4. Window almost full

**Implementation:**
```c
void send_pdu(rlc_pdu_t *pdu) {
    pdu_counter++;
    byte_counter += pdu->length;
    
    bool poll = false;
    if (pdu_counter >= PollPDU ||
        byte_counter >= PollByte ||
        is_last_pdu ||
        window_critical) {
        poll = true;
        pdu->P = 1;
        start_t_PollRetransmit();
    }
    
    transmit(pdu);
}
```

**Receiver responds:**
- Receives polled PDU
- Generates STATUS PDU
- Sends ACK_SN and NACK list"

### **Q4: t-Reassembly kab start hota hai?**

**Answer:**
"t-Reassembly tab start hota hai jab out-of-order PDU receive hota hai aur reassembly already running nahi hai.

**Scenarios:**

**Scenario 1: PDU lost in middle**
```
Receive: SN=1, SN=2, SN=4 (SN=3 missing)
→ Start t-Reassembly when SN=4 received
→ Wait for SN=3
```

**Scenario 2: Timer expires**
```
Timer expired → SN=3 declared lost
→ Update RX_Highest_Status
→ Trigger STATUS PDU with NACK=3
→ Move forward (don't wait anymore)
```

**Scenario 3: Missing PDU arrives**
```
SN=3 received before timer expiry
→ Stop t-Reassembly
→ Deliver SN=3, SN=4 in order
→ Update RX_Next
```

**Implementation:**
```c
void on_pdu_received(rlc_pdu_t *pdu) {
    if (pdu->sn > RX_Next) {
        if (!t_reassembly_running) {
            start_t_reassembly();
        }
    }
    
    buffer_pdu(pdu);
    
    // Try in-order delivery
    while (is_complete(RX_Next)) {
        deliver_to_pdcp(buffer[RX_Next]);
        RX_Next++;
    }
    
    if (RX_Next > t_reassembly_target) {
        stop_t_reassembly();
    }
}
```"

### **Q5: STATUS PDU kya hota hai?**

**Answer:**
"STATUS PDU receiver bhejta hai sender ko reception status batane ke liye.

**Contents:**
- ACK_SN: Highest SN received + 1 (cumulative ACK)
- NACK list: SNs not received
- E1, E2, E3: Extension bits

**Trigger:**
- Polled by sender (P=1)
- t-Reassembly expiry
- Missing PDU detected

**Format:**
```
┌─────┬─────┬──────────┬─────────────┐
│ D/C │ CPT │  ACK_SN  │ NACK lists  │
│  0  │ 000 │          │             │
└─────┴─────┴──────────┴─────────────┘
```

**Suppression:**
- t-StatusProhibit prevents flooding
- Can't send STATUS during this timer
- Aggregates multiple triggers"

### **Q6: RLC SDU vs PDU?**

**Answer:**
"
**RLC SDU (Service Data Unit):**
- From PDCP (above layer)
- Variable size
- Whole packet

**RLC PDU (Protocol Data Unit):**
- To MAC (below layer)
- After segmentation/concatenation
- With RLC header

```
PDCP PDU (5000 bytes)
        ↓
    RLC SDU (5000 bytes)
        ↓ Segmentation
    ┌───┴───┐
    ↓       ↓
 RLC PDU 1  RLC PDU 2
 (1500 B)   (1500 B + ...)
        ↓
   MAC SDU's
```"

### **Q7: 5G NR RLC mein kya naya hai LTE se?**

**Answer:**
"
**5G NR RLC changes:**

1. **Out-of-order delivery option:**
   - LTE: Always in-order
   - NR: Configurable (for low latency)
   
2. **Larger SN:**
   - LTE: 10 bits AM
   - NR: 12 or 18 bits AM
   
3. **Pre-processing:**
   - Generate PDUs in advance
   - Reduce latency
   
4. **Removed:**
   - Concatenation removed (done at MAC now)
   - Simplified header
   
5. **Same:**
   - 3 modes (TM/UM/AM)
   - ARQ mechanism
   - Segmentation/Reassembly"

### **Q8: RLC re-establishment kab hota hai?**

**Answer:**
"RLC re-establishment occurs in:

1. **RRC Connection Re-establishment**
2. **Handover** (intra-cell, inter-cell)
3. **PDCP recovery**

**Actions on re-establishment:**
- Reset all state variables
- Stop all timers
- Discard all stored PDUs
- Reset SN counters
- Clear buffers

**For lossless handover:**
- PDCP handles in-order delivery
- RLC just resets

**Implementation:**
```c
void rlc_re_establish() {
    // Stop timers
    stop_t_PollRetransmit();
    stop_t_Reassembly();
    stop_t_StatusProhibit();
    
    // Reset state
    TX_Next = 0;
    TX_Next_Ack = 0;
    RX_Next = 0;
    RX_Next_Status = 0;
    
    // Clear buffers
    clear_tx_buffer();
    clear_rx_buffer();
    
    // PDCP will resync
}
```"

---

## 🎯 RLC Configuration Examples

### **For VoLTE (UM):**
```
Mode: UM
SN length: 6 bits
t-Reassembly: 35 ms
```

### **For Web Browsing (AM):**
```
Mode: AM
SN length: 12 bits
PollPDU: 4
PollByte: 25 KB
t-PollRetransmit: 50 ms
t-Reassembly: 35 ms
t-StatusProhibit: 10 ms
maxRetxThreshold: 32
```

### **For URLLC (5G NR):**
```
Mode: AM
SN length: 12 bits
PollPDU: infinity (1 PDU at a time)
t-PollRetransmit: 5 ms (very short!)
t-Reassembly: 5 ms
maxRetxThreshold: 4
```

---

## ✅ Self-Test

1. ✅ RLC ke 3 modes batao with use cases
2. ✅ AM mein ARQ kaise kaam karta hai?
3. ✅ STATUS PDU mein kya information hoti hai?
4. ✅ Polling kab trigger hoti hai?
5. ✅ t-Reassembly ka purpose?
6. ✅ Segmentation kyun zaroori hai?
7. ✅ 5G NR RLC mein kya new hai?
8. ✅ RLC re-establishment kab hota hai?
9. ✅ Window-based flow control kaise kaam karta hai?
10. ✅ RLC SDU vs PDU mein difference?

---

## 🚀 Next File:
**Read:** `04_PDCP_LAYER_DEEP.md`
