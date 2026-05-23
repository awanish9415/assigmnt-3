# 🎓 BASICS - Zero Level Foundation

## 🌐 Mobile Network Evolution

### **Generations Timeline:**
```
1G (1980s) → Analog voice
2G (1990s) → Digital voice + SMS (GSM)
3G (2000s) → Mobile data (UMTS, WCDMA)
4G (2010s) → High-speed data (LTE)
5G (2020s) → IoT, low-latency (NR)
6G (Future) → Even faster, AI-driven
```

### **Q: 4G vs 5G mein kya difference hai?**

**Answer:**
| Feature | 4G LTE | 5G NR |
|---------|--------|-------|
| Speed | 100 Mbps - 1 Gbps | 1-20 Gbps |
| Latency | 30-50 ms | 1-10 ms |
| Connections | 100K/km² | 1M/km² |
| Frequency | <6 GHz | <6 GHz + mmWave |
| Use Cases | Mobile broadband | IoT, AR/VR, autonomous |
| Architecture | EPC | 5GC (Service-based) |

---

## 📡 LTE Network Architecture

```
┌─────────┐
│   UE    │ (User Equipment - Mobile phone)
└────┬────┘
     │ Uu Interface (Air interface)
     ↓
┌─────────┐
│  eNB    │ (eNodeB - Base Station)
└────┬────┘
     │ S1 Interface
     ↓
┌─────────┐    ┌─────────┐    ┌─────────┐
│   MME   │←→  │  S-GW   │←→  │  P-GW   │
└─────────┘    └─────────┘    └─────────┘
     EPC (Evolved Packet Core)
              │
              ↓
         Internet/PSTN
```

### **Key Components:**

**UE (User Equipment):**
- Mobile phone
- Modem chipset
- Runs UE protocol stack

**eNB (eNodeB):**
- Base station for LTE
- Handles radio interface
- Implements RAN protocol stack
- Connects UE to core network

**MME (Mobility Management Entity):**
- Control plane signaling
- Authentication
- Mobility management
- Bearer management

**S-GW (Serving Gateway):**
- User plane data routing
- Mobility anchor
- LTE-LTE handovers

**P-GW (PDN Gateway):**
- IP address allocation
- Internet connectivity
- Charging

---

## 📡 5G NR Network Architecture

```
┌─────────┐
│   UE    │
└────┬────┘
     │ Uu Interface
     ↓
┌─────────┐
│  gNB    │ (gNodeB - 5G Base Station)
└────┬────┘
     │ NG Interface
     ↓
   ┌──┴──┐
   ↓     ↓
┌─────┐ ┌─────┐
│ AMF │ │ UPF │
└──┬──┘ └──┬──┘
   │       │
   ↓       ↓
   5G Core (5GC)
   │
   ↓
Internet/Services
```

### **Key Differences from LTE:**

**gNB (5G Base Station):**
- More advanced than eNB
- Can split: CU + DU + RU (ORAN)
- Higher frequencies support

**5GC Components:**

**AMF (Access and Mobility Management Function):**
- Like MME in LTE
- Authentication, mobility

**SMF (Session Management Function):**
- Session management
- IP allocation

**UPF (User Plane Function):**
- Like S-GW + P-GW combined
- Data forwarding

---

## 🏗️ Protocol Stack Layers

### **OSI vs Mobile Network:**

```
┌──────────────────┐
│    Application   │ Layer 7 (HTTP, FTP, etc.)
├──────────────────┤
│   Presentation   │ Layer 6 (Encryption)
├──────────────────┤
│      Session     │ Layer 5 (NAS in LTE)
├──────────────────┤
│     Transport    │ Layer 4 (TCP/UDP)
├──────────────────┤
│      Network     │ Layer 3 (IP, RRC)
├──────────────────┤
│    Data Link     │ Layer 2 (PDCP/RLC/MAC)  ← TUMHARA EXPERTISE!
├──────────────────┤
│     Physical     │ Layer 1 (PHY)
└──────────────────┘
```

### **LTE/5G NR Protocol Stack (User Plane):**

```
         UE                       eNB/gNB
    ┌─────────┐                ┌─────────┐
    │   IP    │                │   IP    │
    └────┬────┘                └────┬────┘
         │                          │
    ┌────▼────┐                ┌────▼────┐
    │  PDCP   │←──────────────→│  PDCP   │ Layer 2
    └────┬────┘                └────┬────┘
         │                          │
    ┌────▼────┐                ┌────▼────┐
    │   RLC   │←──────────────→│   RLC   │ Layer 2
    └────┬────┘                └────┬────┘
         │                          │
    ┌────▼────┐                ┌────▼────┐
    │   MAC   │←──────────────→│   MAC   │ Layer 2 ⭐
    └────┬────┘                └────┬────┘
         │                          │
    ┌────▼────┐                ┌────▼────┐
    │   PHY   │←──radio───────→│   PHY   │ Layer 1
    └─────────┘                └─────────┘
```

**Yeh tumhara expertise hai!** ⭐

---

## 🎯 Layer 2 Components

### **PDCP (Packet Data Convergence Protocol):**
- **Purpose:** Header compression, security
- **Functions:**
  - IP header compression (RoHC)
  - Encryption/Decryption
  - Integrity protection
  - Sequence numbering
  - In-order delivery
  - Duplicate detection

### **RLC (Radio Link Control):**
- **Purpose:** Reliable data transfer
- **Three Modes:**
  - **TM (Transparent Mode):** No header, broadcast
  - **UM (Unacknowledged Mode):** Streaming (voice)
  - **AM (Acknowledged Mode):** Reliable (web, file transfer)
- **Functions:**
  - Segmentation
  - Reassembly
  - ARQ (in AM mode)
  - Sequence numbering

### **MAC (Medium Access Control):**
- **Purpose:** Resource scheduling, multiplexing
- **Functions:**
  - Scheduling (UL/DL)
  - HARQ (8 processes)
  - RACH procedure
  - Logical channel multiplexing
  - Random access
  - Priority handling

---

## 📋 Common Interview Questions (Basics)

### **Q1: Layer 2 mein kya kya hota hai?**

**Answer:**
"Layer 2 mein 3 sub-layers hain:
1. **PDCP** - Header compression aur security
2. **RLC** - Reliable data transfer (3 modes: TM/UM/AM)
3. **MAC** - Scheduling aur HARQ

Main 5 saal se MAC layer pe kaam kar raha hoon - HARQ processes, RACH procedure, aur scheduler design."

### **Q2: UE protocol stack aur eNB protocol stack mein kya difference?**

**Answer:**
"Both sides pe same protocol stack hota hai:
- UE side: User equipment ka stack
- eNB/gNB side: Base station ka stack

Functions same hote hain but **roles different**:
- UE: Request bhejta hai
- eNB: Grant deta hai aur schedule karta hai"

### **Q3: Control plane aur User plane mein kya difference?**

**Answer:**
"**Control Plane (Signaling):**
- UE-eNB communication setup
- RRC messages
- Connection management
- Mobility (handover)
- Bearer setup

**User Plane (Data):**
- Actual user data transfer
- IP packets
- Application data
- Real-time data

Layer 2 (PDCP/RLC/MAC) **dono planes mein** kaam karta hai."

### **Q4: Logical Channel kya hota hai?**

**Answer:**
"Logical channels MAC layer ke upar hote hain - data type ke according:

**Control Logical Channels:**
- BCCH (Broadcast)
- PCCH (Paging)
- CCCH (Common control)
- DCCH (Dedicated control)

**Traffic Logical Channels:**
- DTCH (Dedicated traffic)
- MTCH (Multicast traffic)

MAC inka multiplexing karta hai transport channels pe."

### **Q5: Transport Channel kya hota hai?**

**Answer:**
"Transport channels MAC aur PHY ke beech mein hote hain:

**Downlink:**
- BCH (Broadcast)
- DL-SCH (Shared - main data)
- PCH (Paging)
- MCH (Multicast)

**Uplink:**
- UL-SCH (Shared - main data)
- RACH (Random access)

MAC scheduler decide karta hai konsa logical channel data konsa transport channel pe map hoga."

### **Q6: Physical Channel?**

**Answer:**
"PHY layer ke channels:

**Downlink:**
- PDCCH - Control info
- PDSCH - Data
- PBCH - Broadcast
- PSS/SSS - Synchronization

**Uplink:**
- PUCCH - Control
- PUSCH - Data
- PRACH - Random access

Yeh PHY layer ka kaam hai, but MAC ko inka knowledge zaroori hai scheduling ke liye."

### **Q7: What is BSR (Buffer Status Report)?**

**Answer:**
"BSR ek MAC Control Element (MAC CE) hai jo UE eNB ko bhejta hai uplink data ki info dene ke liye:

**Purpose:**
- UE batata hai uske paas kitna data hai
- Different priority levels (LCG - Logical Channel Groups)
- Scheduler ko UL grant decide karne mein help

**Types:**
1. **Long BSR** - All 4 LCGs
2. **Short BSR** - 1 LCG
3. **Truncated BSR** - When PDU full
4. **Padding BSR** - When extra space

**Trigger Conditions:**
- New data arrival
- Priority data arrival
- Periodic timer
- Padding available"

### **Q8: PHR (Power Headroom Report)?**

**Answer:**
"PHR ek MAC CE hai jisme UE batata hai available transmit power:

**Purpose:**
- Scheduler ko power info deta hai
- Better link adaptation
- Avoid power-limited UEs ko high MCS dene se

**Formula:**
PHR = P_max - P_used

**Trigger:**
- Periodic timer
- Path loss change
- After power limited transmission"

---

## 🎯 Quick Reference - 3GPP Specs

### **MUST KNOW for Interview:**

| Spec | Topic |
|------|-------|
| **3GPP TS 36.321** | LTE MAC |
| **3GPP TS 36.322** | LTE RLC |
| **3GPP TS 36.323** | LTE PDCP |
| **3GPP TS 36.331** | LTE RRC |
| **3GPP TS 38.321** | 5G NR MAC |
| **3GPP TS 38.322** | 5G NR RLC |
| **3GPP TS 38.323** | 5G NR PDCP |
| **3GPP TS 38.331** | 5G NR RRC |

### **Quote in interview:**
*"Per 3GPP TS 38.321 section 5.1, RACH procedure mein..."*

**Yeh expertise dikhata hai!** ⭐

---

## 🎯 Key Acronyms (MUST KNOW)

| Acronym | Full Form |
|---------|-----------|
| MAC | Medium Access Control |
| RLC | Radio Link Control |
| PDCP | Packet Data Convergence Protocol |
| RRC | Radio Resource Control |
| HARQ | Hybrid Automatic Repeat reQuest |
| RACH | Random Access Channel |
| BSR | Buffer Status Report |
| PHR | Power Headroom Report |
| TBS | Transport Block Size |
| MCS | Modulation and Coding Scheme |
| PRB | Physical Resource Block |
| RB | Resource Block |
| TTI | Transmission Time Interval |
| CQI | Channel Quality Indicator |
| RI | Rank Indicator |
| PMI | Precoding Matrix Indicator |
| AMC | Adaptive Modulation and Coding |
| OFDMA | Orthogonal Frequency Division Multiple Access |
| SC-FDMA | Single Carrier FDMA |
| FDD | Frequency Division Duplex |
| TDD | Time Division Duplex |
| eNB | evolved NodeB (LTE base station) |
| gNB | gNodeB (5G base station) |
| EPC | Evolved Packet Core |
| 5GC | 5G Core |
| AMF | Access and Mobility Function |
| UPF | User Plane Function |
| UE | User Equipment |

---

## ✅ Self-Test Questions

Try answering without looking:

1. ✅ Layer 2 ke 3 sub-layers kaun se hain?
2. ✅ RLC ke 3 modes kaun se hain?
3. ✅ MAC ke main functions kya hain?
4. ✅ HARQ kya hai aur kitne processes hote hain?
5. ✅ RACH procedure ke 4 messages kya hain?
6. ✅ BSR aur PHR mein kya difference hai?
7. ✅ 4G aur 5G mein 5 differences batao
8. ✅ eNB aur gNB mein kya difference?
9. ✅ Logical, Transport, Physical channels mein difference?
10. ✅ 3GPP TS 38.321 kya specify karta hai?

---

## 🚀 Next File:
**Read:** `02_MAC_LAYER_DEEP.md`

**Yahan basics clear ho gaye? Ab MAC layer mein deep dive karte hain!** 🎯
