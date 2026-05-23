# 🎯 PDCP Layer - Complete Deep Dive

## 🎯 PDCP Overview

**PDCP = Packet Data Convergence Protocol**
- 3GPP TS 36.323 (LTE) / TS 38.323 (5G NR)
- Top sub-layer of Layer 2
- Between RRC/IP and RLC

### **Position:**
```
┌──────────┐   ┌──────────┐
│   IP     │   │   RRC    │
│ (User)   │   │ (Control)│
└────┬─────┘   └────┬─────┘
     │              │
     └──────┬───────┘
            ↓
       ┌────────┐
       │  PDCP  │ ⭐
       └────┬───┘
            ↓
       ┌────────┐
       │  RLC   │
       └────────┘
```

---

## 🎯 PDCP Functions

### **1. Header Compression (RoHC)**
- Compress IP/UDP/TCP headers
- Used for VoIP, IPv6
- Saves bandwidth

### **2. Security**
- **Ciphering** (Encryption)
- **Integrity Protection**
- For both control and user plane

### **3. Sequence Numbering**
- Detect lost packets
- Duplicate detection
- In-order delivery

### **4. Reordering**
- Buffer out-of-order packets
- Deliver in sequence to higher layer

### **5. Duplicate Elimination**
- Especially in handover
- Same data may arrive twice

### **6. Discard**
- Discard old packets (timer-based)
- discardTimer

### **7. Status Reporting**
- After re-establishment
- Helps lossless handover

### **8. PDCP Duplication (5G NR)**
- Send via multiple paths
- Carrier Aggregation, Dual Connectivity
- Improves reliability

---

## 📊 PDCP PDU Structure

### **Data PDU:**

```
┌────┬────┬──────────┬──────────┬──────┐
│D/C │ R  │   SN     │  Data    │ MAC-I│
└────┴────┴──────────┴──────────┴──────┘

D/C = Data/Control bit
SN = Sequence Number (12 or 18 bits)
MAC-I = Message Authentication Code (32 bits)
        Only for control plane
```

### **Control PDU:**

```
┌────┬─────┬──────────────────┐
│D/C │ Type│   Status info    │
└────┴─────┴──────────────────┘

PDU Types:
- 000: Status Report
- 001: ROHC Feedback
```

---

## 🔐 Security in PDCP

### **Ciphering:**

**Algorithm Examples:**
- EEA0 (NULL) - No encryption
- EEA1 (SNOW 3G)
- EEA2 (AES-CTR)
- EEA3 (ZUC)

**Inputs:**
- Key (KUPenc for user plane, KRRCenc for control)
- COUNT (HFN + SN)
- BEARER ID
- DIRECTION (UL/DL)
- LENGTH

```c
ciphered_data = AES_CTR(
    key,
    count,
    bearer,
    direction,
    plaintext_data
);
```

### **Integrity Protection (Control Plane Only):**

**Algorithm Examples:**
- EIA0 (NULL)
- EIA1 (SNOW 3G)
- EIA2 (AES-CMAC)
- EIA3 (ZUC)

**Generates MAC-I (32-bit tag):**
```c
MAC-I = AES_CMAC(
    key,
    count,
    bearer,
    direction,
    message_data
);
// Append MAC-I to PDU
```

### **HFN (Hyper Frame Number):**

```
COUNT = HFN || SN

HFN: Maintained by both sides
SN: In PDCP header

When SN wraps around → Increment HFN
This prevents replay attacks
```

---

## 🔄 Reordering in PDCP

### **Why Needed?**

In handover, RLC re-establishes. Packets may arrive out-of-order via different paths.

### **Reorder Process:**

```c
typedef struct {
    uint32_t RX_NEXT;        // Next expected
    uint32_t RX_DELIV;       // Delivered to upper
    uint32_t RX_REORD;       // Trigger value
    timer_t reordering_timer;
    pdcp_pdu_t buffer[WINDOW_SIZE];
} pdcp_rx_state_t;

void on_pdu_received(pdcp_pdu_t *pdu) {
    uint32_t sn = pdu->sn;
    
    // Discard if duplicate or out of window
    if (already_received(sn) || 
        sn < RX_DELIV - WINDOW_SIZE/2) {
        discard(pdu);
        return;
    }
    
    // Decipher and decompress
    pdu = decipher(pdu);
    pdu = decompress(pdu);
    
    // Buffer
    buffer[sn] = pdu;
    
    // In-order delivery
    while (buffer[RX_DELIV].present) {
        deliver_to_upper(buffer[RX_DELIV]);
        RX_DELIV++;
    }
    
    // Start reordering timer if needed
    if (RX_DELIV < RX_NEXT) {
        if (!t_reordering_running) {
            start_t_reordering();
            RX_REORD = RX_NEXT;
        }
    }
}

void on_t_reordering_expiry() {
    // Force delivery
    deliver_in_order_until(RX_REORD);
    
    if (RX_DELIV < RX_NEXT) {
        // More to wait
        start_t_reordering();
        RX_REORD = RX_NEXT;
    }
}
```

---

## 🎯 PDCP Duplication (5G NR Feature)

### **Concept:**
Send same PDU via 2 paths - one always reaches!

### **Use Cases:**

**1. URLLC (Ultra-Reliable Low-Latency):**
```
PDCP PDU
    ↓
   ╱ ╲
  ╱   ╲
RLC1  RLC2
 ↓     ↓
MAC1  MAC2
 ↓     ↓
Cell1 Cell2 (CA or DC)
```

**2. Reliability:**
- Even if one path fails, other delivers
- Receiver removes duplicates

### **Activation:**
- Configured via RRC
- Activation/Deactivation via MAC CE
- Dynamic enable/disable

```c
if (pdcp_duplication_enabled) {
    pdcp_pdu_t copy = create_copy(pdu);
    send_via_path1(pdu);
    send_via_path2(copy);
} else {
    send_via_primary_path(pdu);
}
```

---

## 🎯 PDCP Functions Summary

### **Transmitter (TX):**

```c
void pdcp_transmit(pdcp_sdu_t *sdu) {
    // 1. Assign SN
    pdcp_pdu_t *pdu = create_pdu();
    pdu->sn = TX_NEXT++;
    pdu->data = sdu->data;
    
    // 2. Header compression (if enabled)
    if (rohc_enabled) {
        pdu->data = rohc_compress(pdu->data);
    }
    
    // 3. Integrity protection (control plane)
    if (control_plane) {
        pdu->mac_i = compute_integrity(pdu);
    }
    
    // 4. Ciphering
    pdu->data = encrypt(pdu->data, key, count);
    
    // 5. Submit to RLC
    rlc_transmit(pdu);
    
    // 6. Start discardTimer
    start_discard_timer(pdu);
    
    // 7. Duplicate if enabled (5G NR)
    if (duplication_enabled) {
        rlc2_transmit(create_copy(pdu));
    }
}
```

### **Receiver (RX):**

```c
void pdcp_receive(pdcp_pdu_t *pdu) {
    // 1. Decipher
    pdu->data = decrypt(pdu->data, key, count);
    
    // 2. Verify integrity (control plane)
    if (control_plane) {
        if (!verify_integrity(pdu)) {
            discard(pdu);
            return;
        }
    }
    
    // 3. Header decompression
    if (rohc_enabled) {
        pdu->data = rohc_decompress(pdu->data);
    }
    
    // 4. Duplicate detection
    if (already_received(pdu->sn)) {
        discard(pdu);
        return;
    }
    
    // 5. Reordering
    add_to_buffer(pdu);
    deliver_in_order();
}
```

---

## 🎯 Real Interview Questions

### **Q1: PDCP ke main functions kya hain?**

**Answer:**
"PDCP ke 8 main functions hain:

1. **Header Compression (RoHC)** - VoIP me bandwidth saving
2. **Ciphering** - Data encryption
3. **Integrity Protection** - Control plane authentication
4. **Sequence Numbering** - Order tracking
5. **Reordering** - In-order delivery
6. **Duplicate Detection** - Especially in handover
7. **Status Reporting** - After re-establishment
8. **Discard** - Old packet cleanup

5G NR me **PDCP Duplication** add hua hai for URLLC reliability."

### **Q2: PDCP me ciphering kaise hoti hai?**

**Answer:**
"PDCP ciphering algorithm-based hai (EEA1/2/3):

**Inputs:**
- Key (KUPenc for user, KRRCenc for control)
- COUNT = HFN || SN
- BEARER ID
- DIRECTION (UL=0, DL=1)
- LENGTH

**Process:**
```c
keystream = AES_CTR(KEY, COUNT, BEARER, DIR, LENGTH);
ciphertext = plaintext XOR keystream;
```

**Why CTR mode?**
- Stream cipher
- No padding needed
- Same length as plaintext
- Easy to parallelize

**HFN role:**
- Prevents SN wrap-around attacks
- COUNT is unique for each PDU
- Both sides maintain HFN"

### **Q3: Integrity protection kab hoti hai?**

**Answer:**
"Integrity protection **only control plane** mein hoti hai (SRBs - Signaling Radio Bearers):

**Why only control?**
- Control messages security-critical
- User data me header mainly
- Computational cost saves
- 5G NR me optional for DRBs (data)

**Process:**
```c
MAC-I = AES_CMAC(
    K_RRCint,    // Integrity key
    COUNT,
    BEARER,
    DIRECTION,
    MESSAGE
);
// 32-bit tag appended to PDU

// Receiver:
expected_MAC_I = compute(received_pdu);
if (expected_MAC_I != received_MAC_I) {
    discard_pdu();  // Tampered!
}
```

**5G NR Update:**
- DRB integrity now optional
- Useful for sensitive data
- Configurable per bearer"

### **Q4: PDCP SN size kya hai?**

**Answer:**
"PDCP SN sizes:

**LTE PDCP:**
- DRB: 12 or 18 bits
- SRB: 5 bits

**5G NR PDCP:**
- DRB: 12 or 18 bits  
- SRB: 12 bits (always)

**Why different sizes?**
- 12 bits = 4096 SN range
- 18 bits = 262144 SN range
- Higher data rates need larger window
- Trade-off: header size vs window

**Configuration:**
```c
if (high_throughput) {
    pdcp_sn_size = 18;  // Larger window
} else {
    pdcp_sn_size = 12;  // Smaller header
}
```"

### **Q5: PDCP Duplication kya hai?**

**Answer:**
"PDCP Duplication 5G NR ka feature hai for **URLLC reliability**:

**Concept:**
Same PDU 2 paths se bhejo - one always succeeds!

**Architecture:**
```
   PDCP
    ↓
   ╱ ╲
  ↓   ↓
RLC1  RLC2
  ↓   ↓
MAC1  MAC2
  ↓   ↓
Cell1 Cell2  (CA or DC)
```

**Use Cases:**
1. **URLLC** - Industrial automation
2. **Carrier Aggregation** - Reliability
3. **Dual Connectivity** - Path diversity

**Implementation:**
```c
void send_pdu_with_duplication(pdcp_pdu_t *pdu) {
    if (duplication_active) {
        pdcp_pdu_t *copy = clone(pdu);
        rlc_primary->send(pdu);
        rlc_secondary->send(copy);
    } else {
        rlc_primary->send(pdu);
    }
}
```

**At Receiver:**
- Same SN se duplicate detect
- Only first kept, second discarded
- Higher reliability achieved

**Activation:**
- RRC configures
- MAC CE activates/deactivates
- Dynamic per bearer"

### **Q6: PDCP me RoHC kya hai?**

**Answer:**
"RoHC = Robust Header Compression
- IP/UDP/RTP headers compress karta hai
- VoIP ke liye especially useful

**Why?**
- IPv6 header: 40 bytes
- VoIP payload: 30 bytes
- Without compression: header > payload!
- With RoHC: 1-3 bytes header

**Profiles:**
- Profile 0x0001: RTP/UDP/IP
- Profile 0x0002: UDP/IP
- Profile 0x0004: IP only

**Process:**
1. Static fields detected (don't change)
2. Dynamic fields tracked
3. Compressed PDUs sent
4. Receiver decompresses using context

**State Machine:**
- IR (Initialization & Refresh)
- FO (First Order)
- SO (Second Order) - max compression"

### **Q7: PDCP discard timer kab use hota hai?**

**Answer:**
"Discard timer prevents stale data delivery:

**Concept:**
- Each PDU me discardTimer start hota hai
- Expired = delete from buffer
- No retransmission of expired PDUs

**Configuration:**
```
discardTimer:
- ms10, ms20, ms30, ms40, ms50, ms60
- ms75, ms100, ms150, ms200, ms250
- ms300, ms500, ms750, ms1500, infinity
```

**Why important?**
- Real-time data has age limit
- VoIP: 150-200ms max
- Old data useless
- Memory savings

**Implementation:**
```c
void pdcp_send(pdcp_sdu_t *sdu) {
    pdcp_pdu_t *pdu = process(sdu);
    transmit(pdu);
    
    // Start discard timer
    start_timer(pdu, DISCARD_TIMER_VALUE);
}

void on_discard_timer_expiry(pdcp_pdu_t *pdu) {
    if (!pdu->delivered) {
        remove_from_buffer(pdu);
        // Don't retransmit
    }
}
```"

### **Q8: PDCP re-establishment me kya hota hai?**

**Answer:**
"PDCP re-establishment in handover:

**Triggers:**
- Handover (intra/inter cell)
- RRC re-establishment
- Security key change

**For DRB (Data):**
```c
void pdcp_re_establish_drb() {
    // Don't reset SNs
    // Continue from last SN
    
    // For AM RLC:
    // Retransmit unacknowledged PDCP PDUs
    retransmit_outstanding_pdus();
    
    // For UM RLC:
    // Discard buffered PDUs
    discard_buffer();
}
```

**For SRB (Signaling):**
```c
void pdcp_re_establish_srb() {
    // Reset SNs to 0
    TX_NEXT = 0;
    RX_NEXT = 0;
    
    // Discard all PDUs
    discard_all();
    
    // Update keys
    derive_new_keys();
}
```

**Status Report:**
- After re-establishment
- Tells sender which PDUs received
- Avoids retransmission of received PDUs

**For lossless handover:**
- PDCP buffers PDUs at source
- Forwards to target
- Target delivers in order"

---

## 🎯 LTE vs 5G NR PDCP

| Feature | LTE | 5G NR |
|---------|-----|-------|
| SN size DRB | 12/18 bits | 12/18 bits |
| SN size SRB | 5 bits | 12 bits |
| Duplication | No | **Yes** ⭐ |
| Out-of-order delivery | Optional | Configurable |
| DRB integrity | No | Optional |
| Pre-processing | Limited | Enhanced |

---

## ✅ Self-Test

1. ✅ PDCP ke 8 functions batao
2. ✅ Ciphering kaise hoti hai?
3. ✅ Integrity protection kab use hoti hai?
4. ✅ HFN ka role kya hai?
5. ✅ PDCP Duplication kya hai aur kab use karte hain?
6. ✅ RoHC ka purpose?
7. ✅ Discard timer kyun zaroori?
8. ✅ Re-establishment kab hota hai?
9. ✅ PDCP SN size kya kya hote hain?
10. ✅ LTE vs 5G PDCP me 5 differences batao

---

## 🚀 Next File:
**Read:** `05_MAC_SCHEDULER.md`
