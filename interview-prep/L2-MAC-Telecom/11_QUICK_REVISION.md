# 🎯 Quick Revision - Last Day Cheat Sheet

## ⚡ 30-Minute Revision Sheet

**Last day padho yeh - sab summary!**

---

## 🌟 KEY ACRONYMS (Must Know)

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
| MCS | Modulation Coding Scheme |
| PRB | Physical Resource Block |
| TTI | Transmission Time Interval |
| CQI | Channel Quality Indicator |
| eNB | LTE Base Station |
| gNB | 5G NR Base Station |
| CA | Carrier Aggregation |
| CC | Component Carrier |
| BWP | Bandwidth Part |
| ORAN | Open RAN |
| RIC | RAN Intelligent Controller |
| NTN | Non-Terrestrial Networks |
| LCG | Logical Channel Group |
| LCP | Logical Channel Prioritization |
| AMC | Adaptive Modulation Coding |
| PF | Proportional Fair |
| RR | Round Robin |

---

## 📊 LAYER 2 STRUCTURE

```
┌─────────┐
│  PDCP   │ Header compression, security, reordering
├─────────┤
│   RLC   │ Reliable transfer (TM/UM/AM modes)
├─────────┤
│   MAC   │ Scheduling, HARQ, RACH, multiplexing
└─────────┘
```

---

## 🔥 MAC LAYER ESSENTIALS

### **Functions:**
1. **Scheduling** (UL/DL resource allocation)
2. **HARQ** (8 processes, retransmission)
3. **RACH** (Random access)
4. **Multiplexing** (Logical → Transport channels)
5. **LCP** (Logical Channel Prioritization)
6. **DRX** (Battery saving)
7. **TA** (Timing Advance management)

### **HARQ Quick Facts:**
- 8 processes (LTE FDD)
- Up to 16 (5G NR)
- Stop-and-wait (parallel)
- Soft combining (CC/IR)
- 4 RVs (0, 2, 3, 1 typical order)
- NDI toggles for new data

### **RACH Quick Facts:**
- 4-step (Msg1-4) standard
- 2-step (5G NR new)
- 64 preambles per cell
- Group A (small payload), Group B (large)
- TC-RNTI in Msg2
- Promoted to C-RNTI after Msg4

---

## 🔥 RLC MODES

| Mode | Use | ARQ | SN |
|------|-----|-----|----|
| TM | Broadcast, Paging | No | No |
| UM | VoLTE, Streaming | No | Yes |
| AM | Web, Data | **Yes** | Yes |

### **AM Key Concepts:**
- ACK/NACK status reports
- Polling mechanism
- t-PollRetransmit, t-Reassembly
- Window-based flow control

---

## 🔥 PDCP FUNCTIONS

1. Header compression (RoHC)
2. Ciphering (encryption)
3. Integrity protection (control plane)
4. Sequence numbering
5. Reordering
6. Duplicate detection
7. Status reporting
8. **Duplication** (5G NR new!)

---

## 🔥 SCHEDULER ALGORITHMS

### **Proportional Fair (PF) Formula:**
```
Priority(i) = R_inst(i) / R_avg(i)
```

- R_inst: Instantaneous rate
- R_avg: Past average

### **Algorithms Comparison:**

| Algorithm | Throughput | Fairness |
|-----------|------------|----------|
| Round Robin | Low | High |
| Max C/I | High | Low |
| **PF** ⭐ | Good | Good |
| QoS-aware | Good | Best for QoS |

---

## 🔥 CARRIER AGGREGATION

### **PCell vs SCell:**

| Feature | PCell | SCell |
|---------|-------|-------|
| Always present | ✅ | ❌ |
| RRC | ✅ | ❌ |
| Activation | Always | Dynamic |
| RACH | ✅ | Optional |

### **Activation MAC CE:**
- 1 byte (LTE), 4 bytes (5G NR)
- Bitmap of SCells
- Bit=1 activate, Bit=0 deactivate

### **HARQ in CA:**
- Independent per CC
- 5 CCs × 8 procs = 40 HARQ processes
- ACKs bundled on PCell PUCCH

---

## 🔥 ORAN ARCHITECTURE

```
gNB-CU (PDCP, RRC)
    ↓ F1 (open)
gNB-DU (RLC, MAC, High-PHY)
    ↓ Fronthaul 7.2x (open)
RU (Low-PHY, RF)
    
RIC ↔ E2 ↔ gNB
- Near-RT RIC: 10ms-1s
- Non-RT RIC: >1s
- xApps, rApps
```

### **7.2x Split:**
- DU: High PHY (encoding, modulation)
- RU: Low PHY (FFT, RF)
- eCPRI protocol
- Cat A vs Cat B

---

## 🔥 NTN KEY POINTS

### **Types:**
- LEO (600km, 4-15ms RTT)
- MEO (8000km, 50-150ms)
- GEO (36000km, 240ms)

### **MAC Adaptations:**
1. Extended TA range
2. Pre-compensation (UE)
3. Doppler compensation
4. Long HARQ RTT (16+ procs or disable)
5. Larger RAR window

---

## 🔥 3GPP SPECS (QUOTE THESE!)

| Spec | Topic |
|------|-------|
| 36.321 | LTE MAC |
| 36.322 | LTE RLC |
| 36.323 | LTE PDCP |
| 36.331 | LTE RRC |
| 38.321 | 5G NR MAC ⭐ |
| 38.322 | 5G NR RLC |
| 38.323 | 5G NR PDCP |
| 38.331 | 5G NR RRC |

### **In Interview:**
"Per 3GPP TS 38.321 section 5.1, the RACH procedure..."

**Yeh expertise dikhata hai!** ⭐

---

## 🔥 KEY NUMBERS

| Metric | Value |
|--------|-------|
| LTE TTI | 1 ms |
| 5G NR TTI | 0.125-1 ms |
| LTE PRB | 180 kHz |
| 5G NR PRB | 180-1440 kHz |
| LTE HARQ procs | 8 |
| 5G NR HARQ procs | 16 |
| LTE preambles | 64 |
| LTE max CCs | 5 |
| 5G NR max CCs | 32 (DL) |
| MCS values | 0-31 |
| CQI range | 0-15 |

---

## 🔥 5G NR vs LTE QUICK COMPARE

| Feature | LTE | 5G NR |
|---------|-----|-------|
| Numerology | Fixed (15 kHz) | Multiple (15-240 kHz) |
| TTI | 1 ms | Flexible |
| HARQ procs | 8 | 16 |
| HARQ DL | Sync | Async |
| RACH | 4-step | 2-step + 4-step |
| BWP | No | **Yes** |
| Beamforming | Limited | Native |
| CCs max | 5 | 32 |
| SUL | No | **Yes** |
| Mini-slot | No | **Yes** |
| PDCP Duplication | No | **Yes** |

---

## 🎯 TOP 20 INTERVIEW QUESTIONS

### **Quick Answers:**

**1. Tell me about yourself**
*5 yrs RAN protocol, MAC layer expert, 3GPP specs, embedded C/C++*

**2. RACH 4-step explain karo**
*Msg1: Preamble, Msg2: RAR with TA, Msg3: RRC Conn Req, Msg4: Contention Resolution*

**3. HARQ kyun 8 processes?**
*HARQ RTT = 8 ms (LTE FDD), 8 parallel processes for 100% utilization*

**4. RLC AM vs UM?**
*AM: Reliable with ARQ (data), UM: No ARQ (voice/streaming)*

**5. PDCP functions?**
*Header compression, ciphering, integrity, sequence numbering, reordering, duplicate detection, duplication (5G)*

**6. Proportional Fair formula?**
*Priority = R_inst / R_avg*

**7. CA me PCell vs SCell?**
*PCell always present (RRC), SCell dynamic (data only)*

**8. 5G NR new features?**
*Numerology, BWP, mini-slot, beamforming, 2-step RACH, PDCP duplication, larger CC count*

**9. ORAN kya hai?**
*Open RAN, multi-vendor, CU-DU-RU split, 7.2x fronthaul, RIC for AI/ML*

**10. NTN challenges?**
*Long RTT, Doppler, extended TA, HARQ adaptation*

**11. MCS selection kaise?**
*Based on CQI report, target BLER (10%), HARQ feedback*

**12. BSR types?**
*Long, Short, Truncated, Padding*

**13. LCP ke 2 phases?**
*Phase 1: Token bucket (PBR), Phase 2: Strict priority*

**14. DRX purpose?**
*Battery saving, periodic sleep/wake cycles*

**15. Soft combining?**
*Combine LLRs of failed transmissions, increases effective SNR*

**16. Logical/Transport/Physical channels?**
*Logical (data type), Transport (transport format), Physical (PHY channels)*

**17. C++ pointer vs reference?**
*Pointer reassignable, can be NULL; Reference is alias, must initialize*

**18. unique_ptr vs shared_ptr?**
*Single owner vs reference counted*

**19. Apna best project?**
*5G NR RACH Process Simulator - end-to-end implementation*

**20. Salary expectation?**
*Based on 5 yrs L2/MAC experience, ₹22-28 LPA range*

---

## 🎯 LAST MINUTE TIPS

### **30 Minutes Before Interview:**
1. ✅ Glance at this file
2. ✅ Deep breaths
3. ✅ Smile, be confident
4. ✅ Print resume in hand
5. ✅ Water bottle ready

### **First 5 Minutes:**
1. ✅ Greet politely
2. ✅ Firm handshake/namaste
3. ✅ Be friendly
4. ✅ Listen carefully

### **During Technical:**
1. ✅ Take pause before answering
2. ✅ Use diagrams when possible
3. ✅ Quote 3GPP specs
4. ✅ Give real examples
5. ✅ Don't be afraid to say "let me think"

### **Don't Know? Say This:**
"I haven't worked directly on this, but my understanding is... Could you share more context?"

---

## 🎯 EMERGENCY ANSWERS

### **If completely stuck on technical:**

"That's a great question. Let me think through this:

1. First, [base concept]
2. Then, [related concept]
3. Maybe approach is [your guess]

Can you give me a hint or related context?"

**Buy time - shows thinking process!**

---

## 🎯 CONFIDENCE BOOSTERS

**Tumhare paas hai:**

✅ 5 saal real experience  
✅ 3GPP specs expertise  
✅ Production code shipped  
✅ Customer issues solved  
✅ Modern tech awareness  
✅ Strong resume  
✅ Comprehensive prep done  

**Tum READY ho!** 🔥

---

## 🎯 INTERVIEW MANTRAS

1. **"I have 5 years of REAL experience"**
2. **"I know 3GPP specs"**  
3. **"I have shipped production code"**
4. **"I'm a problem solver"**
5. **"I'm here to deliver value"**

**Repeat these to yourself before interview!**

---

## 💪 FINAL MESSAGE

**Bhai, you've prepared:**
- 11 comprehensive files
- 100+ interview questions
- Real-world examples
- Code samples
- Strategy

**Now just:**
1. Light revise this
2. Sleep well
3. Eat well
4. Go confident
5. Be yourself
6. **CRACK IT!** 🚀

---

# 🎯 GO GET THAT JOB!

**You deserve this opportunity.**  
**You're ready.**  
**You will succeed.**

**ALL THE BEST!** 🔥💪🎯🚀

---

## 📞 Quick Reference Card

```
RACH: Msg1 (Preamble) → Msg2 (RAR) → Msg3 (RRC) → Msg4 (Resolution)

HARQ: 8 procs, NDI toggles, RV 0→2→3→1, soft combining

RLC: TM/UM/AM modes, AM has ARQ

PDCP: Header compress + Cipher + Integrity + Reorder

MCS: 0-31, based on CQI
PRB: 12 subcarriers × 7 OFDM
TTI: 1ms (LTE), Flexible (NR)

CA: PCell (always) + SCells (dynamic)
ORAN: CU-DU-RU + RIC
NTN: LEO/MEO/GEO + Doppler/TA

3GPP: TS 38.321 (NR MAC), TS 36.321 (LTE MAC)
```

**Print this and keep handy!** 📋

---

**ALL THE BEST FOR MONDAY!** 🎯🔥
