# Level 8: QoS, DRB, 5QI 82/83, DC-GBR, Network Slicing, NSSAI

> JD explicitly mentions: "5QI 82/83 DC-GBR profile configuration", "Dedicated DRB config", "Network slicing/NSSAI/DNN", "QoS policy (DC-GBR flows)"

---

## SECTION A: QoS Basics (1-15)

### Q1. What is QoS?
**Answer:** Quality of Service - giving different priority and treatment to different traffic types. Voice needs low latency, file download needs high throughput.

### Q2. What QoS model does 5G use?
**Answer:** 5G uses **QoS Flow** model. Each PDU session can have multiple QoS Flows. Each flow has:
- **QFI** (QoS Flow ID).
- **5QI** (standardized profile).
- **ARP** (Allocation and Retention Priority).
- Optional: GFBR, MFBR, AveragingWindow.

### Q3. What is QFI?
**Answer:** QFI (QoS Flow Identifier) - 6-bit value (0-63) identifying a QoS Flow inside a PDU session.

### Q4. What is 5QI?
**Answer:** 5QI (5G QoS Identifier) - integer pointing to a standardized QoS profile. Defines:
- Priority Level
- Packet Delay Budget (PDB)
- Packet Error Rate (PER)
- Default Maximum Data Burst Volume (for DC-GBR)
- Default Averaging Window (for GBR)

### Q5. What are the categories of 5QI?
**Answer:** Three resource types:
1. **GBR (Guaranteed Bit Rate):** Voice, video call. 5QI 1-4.
2. **Non-GBR:** Best effort. 5QI 5-9, 79.
3. **Delay-Critical GBR (DC-GBR):** Ultra-low latency. 5QI 82-87.

### Q6. List some standard 5QI values.
**Answer:**
| 5QI | Type | Priority | PDB | PER | Example |
|-----|------|----------|-----|-----|---------|
| 1 | GBR | 20 | 100ms | 10^-2 | Voice |
| 2 | GBR | 40 | 150ms | 10^-3 | Video call |
| 3 | GBR | 30 | 50ms | 10^-3 | Real-time game |
| 4 | GBR | 50 | 300ms | 10^-6 | Non-conv video |
| 5 | Non-GBR | 10 | 100ms | 10^-6 | IMS signaling |
| 6 | Non-GBR | 60 | 300ms | 10^-6 | Video (buffered) |
| 7 | Non-GBR | 70 | 100ms | 10^-3 | Voice, video |
| 8 | Non-GBR | 80 | 300ms | 10^-6 | TCP-based |
| 9 | Non-GBR | 90 | 300ms | 10^-6 | Default internet |
| 79 | Non-GBR | 65 | 50ms | 10^-2 | V2X messages |
| **82** | **DC-GBR** | **19** | **10ms** | **10^-4** | **Discrete automation** |
| **83** | **DC-GBR** | **22** | **10ms** | **10^-4** | **Discrete automation** |
| 84 | DC-GBR | 24 | 30ms | 10^-5 | Intelligent transport |
| 85 | DC-GBR | 21 | 5ms | 10^-5 | Electricity distribution |
| 86 | DC-GBR | 18 | 5ms | 10^-4 | V2X messages (low latency) |
| 87 | DC-GBR | 25 | 5ms | 10^-3 | Interactive service |

### Q7. What is the difference between GBR and DC-GBR?
**Answer:**
- **GBR:** Guaranteed rate, normal latency (50-300ms).
- **DC-GBR:** Guaranteed rate + delay critical (5-30ms). Designed for URLLC.

Also DC-GBR has a Maximum Data Burst Volume parameter, GBR does not.

### Q8. What is 5QI 82 used for?
**Answer:** **Discrete Automation** (e.g., factory floor robots, motion control). 
- Priority: 19 (lower number = higher priority).
- Packet Delay Budget: 10 ms.
- Packet Error Rate: 10^-4.
- Max Data Burst Volume: 255 bytes.

### Q9. What is 5QI 83 used for?
**Answer:** Also **Discrete Automation** but slightly less strict.
- Priority: 22.
- PDB: 10 ms.
- PER: 10^-4.
- Max Data Burst Volume: 1354 bytes (larger packets).

### Q10. What is ARP (Allocation and Retention Priority)?
**Answer:** ARP has 3 parts:
- **Priority Level:** 1-15 (1 highest).
- **Pre-emption Capability:** Yes/No - can this flow kick out others?
- **Pre-emption Vulnerability:** Yes/No - can this flow be kicked out?

ARP is used when resources are scarce (congestion).

### Q11. What is GFBR and MFBR?
**Answer:**
- **GFBR (Guaranteed Flow Bit Rate):** Minimum guaranteed rate for a GBR flow.
- **MFBR (Maximum Flow Bit Rate):** Upper limit. Excess can be dropped or downgraded.

### Q12. What is Session AMBR and UE AMBR?
**Answer:**
- **Session AMBR:** Aggregate bit rate across all non-GBR flows in one PDU session.
- **UE AMBR:** Total across all PDU sessions for one UE (for non-GBR only).

GBR flows are NOT counted in AMBR.

### Q13. Where is each QoS parameter enforced?
**Answer:**
| Parameter | Enforced By |
|-----------|-------------|
| 5QI | gNB (scheduler), UPF |
| MFBR | UPF (and gNB) |
| GFBR | gNB scheduler |
| Session AMBR | UPF |
| UE AMBR | gNB |

### Q14. What is reflective QoS?
**Answer:** UE learns QoS rules from downlink packets. RQI bit in PDU Session Container header. UE applies same QoS to UL. Saves signaling.

### Q15. What is PCC rule?
**Answer:** PCC (Policy and Charging Control) rule - defines:
- SDF (Service Data Flow) filter (5-tuple).
- QoS (5QI, MBR, GBR, ARP).
- Charging info.

Created in PCF, enforced in UPF (via SMF).

---

## SECTION B: DRB and Bearer Concepts (16-25)

### Q16. What is DRB?
**Answer:** DRB (Data Radio Bearer) - logical pipe carrying user data over the radio (Uu) between UE and gNB. Each has its own PDCP, RLC, MAC config.

### Q17. How are QoS Flows mapped to DRBs?
**Answer:** SDAP at gNB does mapping:
- Multiple QoS Flows can map to same DRB.
- But one QFI cannot be in two DRBs simultaneously.
- Common: one DRB per QFI for simplicity, or group similar QFIs.

### Q18. What is default DRB?
**Answer:** First DRB created when PDU session is established. Carries default flow (catch-all).

### Q19. What is dedicated DRB?
**Answer:** DRB created for specific QoS requirement. E.g., a 5QI 82 DRB for ultra-low-latency robotics traffic, separate from default DRB.

### Q20. How to configure a dedicated DRB?
**Answer:**
1. PCF creates PCC rule with 5QI 82 + SDF filter.
2. SMF triggers PDU Session Modification.
3. SMF -> UPF: PFCP Session Modify with new PDR + QER.
4. SMF -> AMF -> gNB: NGAP Update with new QoS Flow (QFI, 5QI 82).
5. gNB SDAP/PDCP creates new DRB with appropriate RLC mode (likely UM with low latency).
6. RRC Reconfiguration tells UE about new DRB.

### Q21. What RLC mode for DC-GBR (5QI 82/83)?
**Answer:** **RLC UM (Unacknowledged Mode)** because:
- AM retransmissions add latency.
- DC-GBR needs predictable, low-latency.
- Some packet loss acceptable (PER 10^-4).

### Q22. How many DRBs can a UE have?
**Answer:** Up to 32 DRBs per UE (Rel-15). Limit may be lower in implementation.

### Q23. What is the DRB identity?
**Answer:** DRB-Identity: integer 1-32 unique per UE. Allocated by gNB. Carried in RRC Reconfiguration and used as logical channel ID.

### Q24. What happens to packets when DRB fails?
**Answer:**
- PDCP buffer can hold packets.
- Retransmission via PDCP (if AM).
- If radio link fail, RRC Reestablishment.
- Severe failure: PDU session deactivation, then reactivation.

### Q25. What is SDAP header?
**Answer:** 1-byte header (if configured):
- D/C (1 bit): Data or Control.
- R (1 bit): Reserved.
- RQI (1 bit): Reflective QoS Indicator (DL).
- QFI (6 bits).

UL has slightly different bits but same structure.

---

## SECTION C: Network Slicing (26-40)

### Q26. What is Network Slicing?
**Answer:** Network slicing creates multiple **logical networks** on one **physical** 5G infrastructure. Each slice tuned for a specific use case (eMBB, URLLC, mMTC, enterprise).

### Q27. What is S-NSSAI?
**Answer:** Single Network Slice Selection Assistance Information. Identifies one slice. Has two parts:
- **SST (Slice/Service Type):** 8-bit. Standardized values:
  - 1 = eMBB
  - 2 = URLLC
  - 3 = mMTC
  - 4 = V2X
  - 5 = HMTC
- **SD (Slice Differentiator):** 24-bit optional. Operator-defined.

### Q28. What is NSSAI?
**Answer:** NSSAI = collection of S-NSSAIs. Types:
- **Requested NSSAI:** UE asks for slices.
- **Allowed NSSAI:** Returned by AMF (filtered by subscription, location, etc.).
- **Default NSSAI:** Used when no request.
- **Configured NSSAI:** UE stores per PLMN.
- **Subscribed NSSAI:** What UE has in subscription.
- **Rejected NSSAI:** Slices that AMF refused.

### Q29. What is the slice selection flow?
**Answer:**
1. UE includes Requested NSSAI in Registration Request.
2. AMF checks subscription (from UDM) for Subscribed NSSAI.
3. AMF may query NSSF for slice selection.
4. AMF determines Allowed NSSAI.
5. AMF returns in Registration Accept.
6. UE establishes PDU sessions per S-NSSAI.

### Q30. What is the role of NSSF?
**Answer:** NSSF (Network Slice Selection Function):
- Helps AMF select slices for UE.
- Recommends target AMF if needed (AMF re-allocation).
- Resolves slice mapping for roaming.

### Q31. Can different slices use different NFs?
**Answer:** Yes! In particular:
- **AMF:** Can be shared OR per-slice.
- **SMF:** Usually per-slice (or per slice group).
- **UPF:** Per-slice common.
- **PCF:** Can be per-slice.
- **NRF:** Slice info is in NF registration.

### Q32. How is slice info used in routing?
**Answer:** Each PDU session has S-NSSAI. When UE registers, AMF picks SMF based on slice. SMF picks UPF based on slice + DNN. Data flows on slice-specific path.

### Q33. What is DNN?
**Answer:** DNN (Data Network Name) - identifies the data network UE wants to connect to. Examples: "internet", "ims", "enterprise". Replaces APN of 4G.

### Q34. What is the relation between DNN and S-NSSAI?
**Answer:** Same DNN can be in multiple slices. UE specifies both (S-NSSAI + DNN) when creating PDU session. Combination decides UPF, IP pool, QoS.

### Q35. What is "single registration mode" and "dual registration mode"?
**Answer:** When UE has access to both 5G and EPS (4G):
- **Single registration:** UE has one registration (either 5G or 4G) and mobility between them.
- **Dual registration:** UE registers in both simultaneously.

### Q36. What is NEST?
**Answer:** NEST (Network Slice Template) - GSMA standard describing a slice (attributes, KPIs, capabilities). Used by orchestrator to instantiate slices.

### Q37. How is isolation achieved between slices?
**Answer:**
- **Logical:** S-NSSAI separates flows.
- **Resource:** Dedicated UPFs, separate PRB allocation in RAN.
- **NF:** Some NFs are slice-specific.
- **Transport:** Separate VLANs or VPNs in backhaul.

### Q38. What is Slice-aware AMF?
**Answer:** AMF that handles only specific slices. If UE has slice X but AMF doesn't support X, AMF re-routes UE to a different AMF that does. This is "AMF re-allocation".

### Q39. What is the difference between physical and logical slicing?
**Answer:**
- **Logical:** S-NSSAI tags, shared NFs. Cheap, less isolation.
- **Physical:** Dedicated hardware/NFs per slice. Expensive but full isolation.

### Q40. Common slicing test scenarios?
**Answer:**
- 2-3 slices: eMBB (5QI 9), URLLC (5QI 82), mMTC (5QI 80).
- Different DNNs per slice (internet, factory, iot).
- Verify UE can have PDU session per slice.
- Verify QoS differentiation in traffic.

---

## END OF LEVEL 8 (40 questions)

> Move to **09_TIMING_PTP.md**.
