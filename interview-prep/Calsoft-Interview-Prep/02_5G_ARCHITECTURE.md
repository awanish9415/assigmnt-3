# Level 2: 5G Architecture (RAN + Core + Interfaces)

> This level covers complete 5G architecture - the building blocks and how they connect.

---

## SECTION A: 5G Overall Architecture (1-15)

### Q1. Draw and explain the 5G end-to-end architecture.
**Answer:**
```
[UE] <---Uu---> [gNB] <---N2---> [AMF]
                  |              [SMF]
                  +----N3-----> [UPF] <----N6---> [Data Network/Internet]
```
- **UE:** Mobile/IoT device
- **gNB:** 5G base station (RAN)
- **AMF:** Access and Mobility Management Function
- **SMF:** Session Management Function
- **UPF:** User Plane Function
- **N6:** Interface to Data Network (internet)

### Q2. What are the main components of 5G RAN?
**Answer:**
- **gNB:** Single 5G base station, OR can be split into:
  - **CU (Central Unit):** PDCP, RRC, SDAP
  - **DU (Distributed Unit):** RLC, MAC, High-PHY
  - **RU (Radio Unit):** Low-PHY, RF

### Q3. What are the main NFs (Network Functions) in 5G Core?
**Answer:**
| NF | Full Name | Purpose |
|----|-----------|---------|
| AMF | Access and Mobility Mgmt | Registration, mobility |
| SMF | Session Management | PDU sessions, IP allocation |
| UPF | User Plane Function | Data forwarding, GTP-U |
| PCF | Policy Control Function | QoS, charging rules |
| UDM | Unified Data Mgmt | Subscriber data |
| UDR | Unified Data Repository | Database for UDM |
| AUSF | Authentication Server | Authentication |
| NRF | NF Repository Function | NF discovery, registration |
| NSSF | Network Slice Selection | Slice selection |
| NEF | Network Exposure Function | Expose APIs to 3rd party |

### Q4. What is SBA in 5G?
**Answer:** SBA (Service-Based Architecture) means 5G NFs talk to each other using HTTP/2 REST APIs (like microservices). Each NF exposes services. Different from 4G which used point-to-point interfaces.

### Q5. What is the protocol used between 5G Core NFs?
**Answer:** HTTP/2 with JSON, on top of TCP/TLS. Service-Based Interface (SBI). Discovery via NRF.

### Q6. What interfaces exist between RAN and 5GC?
**Answer:**
- **N2:** Control plane (gNB <-> AMF), uses NGAP over SCTP.
- **N3:** User plane (gNB <-> UPF), uses GTP-U over UDP.

### Q7. What is the difference between control plane and user plane in 5G?
**Answer:**
- **Control Plane:** Signaling. Goes UE -> gNB -> AMF -> SMF. Uses NGAP, NAS, HTTP/2.
- **User Plane:** Actual user data. Goes UE -> gNB -> UPF -> Internet. Uses GTP-U.

### Q8. What is CUPS in 5G?
**Answer:** CUPS (Control and User Plane Separation) means user plane (UPF) is separated from control plane (SMF). UPF can be placed at the edge for low latency.

### Q9. What is the difference between 4G EPC and 5G Core?
**Answer:**
| 4G EPC | 5G Core |
|--------|---------|
| MME | AMF + SMF |
| SGW + PGW | UPF |
| HSS | UDM + UDR + AUSF |
| PCRF | PCF |
| Diameter, GTP-C | HTTP/2 (SBA) |
| Monolithic | Service-based, microservices |

### Q10. What is multi-RAT in 5G?
**Answer:** Multi-RAT means using multiple radio technologies together (5G NR + LTE + Wi-Fi). NSA mode is example of multi-RAT.

### Q11. What is dual connectivity in 5G?
**Answer:** UE connects to two base stations at the same time (e.g., LTE + 5G NR in NSA mode = EN-DC).

### Q12. What is EN-DC?
**Answer:** EN-DC (E-UTRAN New Radio Dual Connectivity) - UE connects to both LTE eNB (master) and 5G gNB (secondary). Used in 5G NSA.

### Q13. What is the role of AMF?
**Answer:** AMF handles:
- Registration management
- Connection management
- Mobility management (handover, tracking area updates)
- NAS signaling termination
- Access authentication and authorization (via AUSF)

### Q14. What is the role of SMF?
**Answer:** SMF handles:
- PDU session establishment, modification, release
- UE IP address allocation
- UPF selection and control via N4/PFCP
- QoS policy enforcement
- Traffic steering

### Q15. What is the role of UPF?
**Answer:** UPF handles:
- User data packet forwarding
- GTP-U tunnel termination on N3
- QoS enforcement
- Traffic measurement and reporting
- Packet inspection (DPI)
- Local breakout for edge computing

---

## SECTION B: Interfaces in 5G (16-35)

### Q16. List all the major interfaces in 5G with their protocols.
**Answer:**
| Interface | Between | Protocol |
|-----------|---------|----------|
| Uu | UE - gNB | NR Radio |
| Xn | gNB - gNB | XnAP/SCTP |
| F1 | CU - DU | F1AP (CP), GTP-U (UP) |
| E1 | CU-CP - CU-UP | E1AP/SCTP |
| Open Fronthaul | DU - RU | eCPRI |
| N1 | UE - AMF | NAS (over RRC) |
| N2 | gNB - AMF | NGAP/SCTP |
| N3 | gNB - UPF | GTP-U/UDP |
| N4 | SMF - UPF | PFCP/UDP |
| N6 | UPF - DN | IP (any) |
| N9 | UPF - UPF | GTP-U |
| N11 | AMF - SMF | HTTP/2 |
| N7 | SMF - PCF | HTTP/2 |
| N8 | AMF - UDM | HTTP/2 |
| N12 | AMF - AUSF | HTTP/2 |
| N13 | AUSF - UDM | HTTP/2 |
| N15 | AMF - PCF | HTTP/2 |

### Q17. What is N1 interface?
**Answer:** N1 is the logical interface between UE and AMF. NAS messages travel through this (over RRC over Uu).

### Q18. What is N2 interface and what protocol does it use?
**Answer:** N2 is between gNB and AMF. Protocol: **NGAP** (NG Application Protocol) over **SCTP** (port 38412).

### Q19. What is N3 interface and what protocol does it use?
**Answer:** N3 is between gNB and UPF for user plane. Protocol: **GTP-U** over **UDP** (port 2152).

### Q20. What is N4 interface?
**Answer:** N4 is between SMF and UPF. Protocol: **PFCP** (Packet Forwarding Control Protocol) over UDP (port 8805).

### Q21. What is N6 interface?
**Answer:** N6 is between UPF and Data Network (internet, IMS, enterprise network). Uses standard IP.

### Q22. What is N9 interface?
**Answer:** N9 is between two UPFs (e.g., visited UPF and home UPF in roaming). Uses GTP-U.

### Q23. What is Xn interface?
**Answer:** Xn is between two gNBs. Used for handover and dual connectivity. Protocol: XnAP/SCTP for control, GTP-U for user data.

### Q24. What is F1 interface?
**Answer:** F1 is between CU and DU (in split gNB).
- **F1-C:** Control plane, F1AP over SCTP
- **F1-U:** User plane, GTP-U over UDP

### Q25. What is E1 interface?
**Answer:** E1 is between CU-CP and CU-UP (within CU split). Protocol: E1AP over SCTP.

### Q26. What is the Open Fronthaul (OFH) interface?
**Answer:** Open Fronthaul is between DU and RU in O-RAN. Uses eCPRI. Standardized by O-RAN Alliance (WG4). Splits at 7.2x.

### Q27. What is SCTP and why is it used in 5G?
**Answer:** SCTP (Stream Control Transmission Protocol) is like TCP but better for telecom:
- Multi-streaming (multiple streams in one connection)
- Multi-homing (failover between IPs)
- Message-oriented (not byte-stream)
- Used for NGAP, F1AP, E1AP, XnAP.

### Q28. What is GTP-U?
**Answer:** GTP-U (GPRS Tunneling Protocol - User plane) tunnels user data over IP. Used on N3, N9, F1-U interfaces. Port 2152 UDP.

### Q29. What is GTP-C?
**Answer:** GTP-C (Control plane) was used in 4G for session management. Not used in 5G core (replaced by HTTP/2 and PFCP).

### Q30. What is PFCP?
**Answer:** PFCP (Packet Forwarding Control Protocol) is used on N4 between SMF and UPF. SMF tells UPF how to handle packets (rules for forwarding, QoS, charging).

### Q31. What is NAS?
**Answer:** NAS (Non-Access Stratum) - signaling between UE and AMF. Goes through gNB transparently. Two parts:
- **MM (Mobility Management):** Registration, authentication.
- **SM (Session Management):** PDU session.

### Q32. What is NGAP?
**Answer:** NGAP (NG Application Protocol) is the application layer protocol on N2 between gNB and AMF. Carries UE context, mobility, NAS transport.

### Q33. What is the difference between NGAP and F1AP?
**Answer:**
- **NGAP:** Between gNB and AMF (N2).
- **F1AP:** Between CU and DU (F1 interface).
Both use SCTP underneath.

### Q34. What is RRC?
**Answer:** RRC (Radio Resource Control) is the control protocol between UE and gNB. Handles:
- Connection setup/release
- Mobility (measurement, handover)
- Security activation
- Bearer setup

### Q35. What is the difference between RRC_IDLE, RRC_INACTIVE, RRC_CONNECTED?
**Answer:**
- **RRC_IDLE:** UE not connected, only camped. No RAN context.
- **RRC_INACTIVE:** UE inactive but RAN context kept (fast resume). New in 5G.
- **RRC_CONNECTED:** Active connection with gNB.

---

## SECTION C: CU/DU/RU Split (36-50)

### Q36. What is gNB split architecture?
**Answer:** Instead of one big gNB box, it is split into 3 parts:
- **CU (Central Unit):** PDCP, SDAP, RRC. Can be far from antenna.
- **DU (Distributed Unit):** RLC, MAC, High-PHY. Near to antenna.
- **RU (Radio Unit):** Low-PHY, RF. At antenna site.

### Q37. Why split gNB into CU/DU/RU?
**Answer:**
- **Flexibility:** Different vendors can supply each part (in O-RAN).
- **Cost:** CU can be centralized (cloud), saves space.
- **Performance:** DU close to RU = low latency for processing.
- **Scalability:** Add more DUs without changing CU.

### Q38. What is functional split in 3GPP?
**Answer:** 3GPP defined 8 split options (Option 1 to Option 8). Most common:
- **Option 2:** Between PDCP and RLC (CU-DU split). Standard 3GPP split.
- **Option 7.2x:** Inside PHY layer (DU-RU split). Used by O-RAN.
- **Option 8:** CPRI (between PHY and RF).

### Q39. What is Option 2 split?
**Answer:** CU has PDCP+RRC+SDAP. DU has RLC+MAC+PHY. F1 interface between them. This is the most common CU-DU split.

### Q40. What is Option 7.2x split?
**Answer:** Split inside PHY layer between High-PHY (in DU) and Low-PHY (in RU). Used by O-RAN over Open Fronthaul interface. Uses eCPRI.

### Q41. What is CU-CP and CU-UP?
**Answer:** CU is further split into:
- **CU-CP:** Control Plane (RRC, PDCP-C).
- **CU-UP:** User Plane (PDCP-U, SDAP).
Connected by **E1 interface**.

### Q42. Why split CU into CU-CP and CU-UP?
**Answer:**
- Scale them independently (more users = more UP, more signaling = more CP).
- Place them in different locations.
- Different vendors possible.

### Q43. What is the location strategy for CU, DU, RU?
**Answer:**
- **RU:** Cell site (with antenna).
- **DU:** Cell site or edge data center (within ~20 km of RU).
- **CU:** Regional/central data center (can be 100+ km).

### Q44. What is eCPRI?
**Answer:** eCPRI (Enhanced Common Public Radio Interface) is the protocol used between DU and RU in Open Fronthaul. Lighter than CPRI, uses Ethernet, supports split 7.2x.

### Q45. What is the difference between CPRI and eCPRI?
**Answer:**
- **CPRI:** Carries raw radio samples (Option 8 split). Very high bandwidth.
- **eCPRI:** Carries frequency-domain samples (Option 7.2x). Much lower bandwidth.

### Q46. What is the bandwidth needed for fronthaul?
**Answer:** Depends on cell config. Roughly:
- 100 MHz, 4T4R: ~10-25 Gbps per cell on eCPRI.
- CPRI for same: ~150 Gbps (impractical).

### Q47. What is fronthaul, midhaul and backhaul?
**Answer:**
- **Fronthaul:** RU to DU (eCPRI).
- **Midhaul:** DU to CU (F1 interface).
- **Backhaul:** CU/gNB to 5GC.

### Q48. What is O-RAN's split 7.2 advantage?
**Answer:**
- Lower fronthaul bandwidth.
- Multi-vendor (RU from one vendor, DU from another).
- Open standard (no lock-in).
- Cloud-native DU/CU possible.

### Q49. What is C-plane and U-plane and S-plane in Open Fronthaul?
**Answer:**
- **C-plane:** Control plane messages (scheduling, beamforming) - eCPRI.
- **U-plane:** User plane data (IQ samples) - eCPRI.
- **S-plane:** Synchronization (PTP).
- **M-plane:** Management (NETCONF/YANG).

### Q50. What is the M-plane in Open Fronthaul?
**Answer:** M-plane (Management plane) handles configuration of RU. Uses NETCONF/YANG. Hybrid model (DMS+NMS) or hierarchical model.

---

## SECTION D: Roaming, Handover, Identifiers (51-60)

### Q51. What is roaming in 5G?
**Answer:** When UE moves to another operator's network (e.g., abroad), it uses **VPLMN (Visited PLMN)**. Two models:
- **LBO (Local Breakout):** UPF in visited network.
- **HR (Home Routed):** UPF in home network.

### Q52. What is handover in 5G?
**Answer:** UE moves from one gNB to another while keeping connection. Types:
- **Xn-based:** Direct between gNBs.
- **N2-based:** Via AMF (when no Xn).
- **Intra-CU, inter-DU:** Within same CU.

### Q53. What identifiers are used in 5G?
**Answer:**
- **SUPI:** Permanent ID (IMSI-like).
- **SUCI:** Encrypted SUPI (sent over air).
- **5G-GUTI:** Temporary ID after registration.
- **5G-S-TMSI:** Shortened GUTI for paging.
- **PEI:** Permanent Equipment ID (IMEI).
- **GPSI:** Generic Public Subscription ID (MSISDN, phone number).

### Q54. What is the structure of SUPI?
**Answer:** Two types:
- **IMSI-based SUPI:** MCC + MNC + MSIN.
- **NAI-based SUPI:** username@realm.

### Q55. What is GUAMI?
**Answer:** GUAMI (Globally Unique AMF Identifier) = PLMN ID + AMF Region ID + AMF Set ID + AMF Pointer.

### Q56. What is NCGI?
**Answer:** NCGI (NR Cell Global Identity) = PLMN ID + NR Cell Identity. Identifies a cell globally.

### Q57. What is the registration procedure in 5G?
**Answer:** Steps:
1. UE sends Registration Request (NAS) via gNB to AMF.
2. AMF authenticates UE via AUSF + UDM.
3. AMF queries UDM for subscription data.
4. PDU session can be triggered.
5. AMF sends Registration Accept.

### Q58. What is PDU session establishment procedure?
**Answer:**
1. UE sends PDU Session Establishment Request to SMF (via AMF).
2. SMF selects UPF, allocates IP.
3. SMF configures UPF via N4 (PFCP).
4. AMF sets up N2 session to gNB.
5. gNB sets up DRB for UE.
6. PDU session is active.

### Q59. What is AKA?
**Answer:** AKA (Authentication and Key Agreement) is the security procedure for 5G. Uses SIM card secrets. Two methods:
- **5G-AKA**
- **EAP-AKA'**

### Q60. What is the difference between 5G-AKA and EAP-AKA'?
**Answer:** Both authenticate UE. 5G-AKA is simpler. EAP-AKA' uses EAP framework, supports more access types (Wi-Fi, etc.). Both result in derived keys (KAUSF, KSEAF, etc.).

---

## END OF LEVEL 2 (60 questions)

> Move to **03_ORAN_FRONTHAUL_RAN.md** next for RAN-specific deep dive.
