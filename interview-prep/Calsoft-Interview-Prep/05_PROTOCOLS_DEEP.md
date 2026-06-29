# Level 5: Protocols Deep Dive (SCTP, GTP-U, PFCP, N2/N3, F1, E1)

> JD mentions: "Linux networking: SCTP, GTP-U, TUN/TAP interfaces", "N2/N3 integration", "F1/E1 interfaces". These protocols are 100% asked.

---

## SECTION A: SCTP (1-15)

### Q1. What is SCTP?
**Answer:** SCTP (Stream Control Transmission Protocol) is a transport-layer protocol (Layer 4) like TCP, but designed for telecom signaling. RFC 4960.

### Q2. Why is SCTP used in 5G?
**Answer:** SCTP gives:
- **Reliability:** Like TCP, guarantees delivery.
- **Multi-streaming:** Multiple independent streams in one association (no head-of-line blocking).
- **Multi-homing:** One association can use multiple IP addresses for failover.
- **Message-oriented:** Preserves message boundaries (TCP is byte-stream).
- **Better security:** Protection against SYN-flood (4-way handshake).

### Q3. Where is SCTP used in 5G?
**Answer:**
- **N2 (NGAP)** - gNB to AMF.
- **F1-C (F1AP)** - DU to CU.
- **E1 (E1AP)** - CU-UP to CU-CP.
- **Xn-C (XnAP)** - gNB to gNB.
- **S1-MME (4G)** - eNB to MME.

### Q4. What are SCTP ports for 5G interfaces?
**Answer:**
- **NGAP (N2):** 38412
- **F1AP:** 38472
- **E1AP:** 38462
- **XnAP:** 38422
- **S1AP (4G):** 36412

### Q5. What is an SCTP association?
**Answer:** Like a TCP connection but with more features. Identified by:
- Local IP(s) + local port.
- Remote IP(s) + remote port.
Can have multiple IPs on each side (multi-homing).

### Q6. What is the SCTP 4-way handshake?
**Answer:**
1. **INIT** (client -> server).
2. **INIT-ACK** (server -> client, with cookie).
3. **COOKIE-ECHO** (client -> server, with cookie).
4. **COOKIE-ACK** (server -> client).

Cookie mechanism protects against SYN-flood attacks.

### Q7. What is SCTP multi-homing?
**Answer:** Single association can use multiple IP addresses. If primary path fails, packets switch to backup path. Useful for redundancy in telco networks.

### Q8. What are SCTP chunks?
**Answer:** SCTP packets contain multiple "chunks". Types:
- **DATA:** User data.
- **INIT, INIT-ACK, COOKIE-ECHO, COOKIE-ACK:** Association setup.
- **SACK:** Selective acknowledgment.
- **HEARTBEAT, HEARTBEAT-ACK:** Keep-alive.
- **SHUTDOWN, SHUTDOWN-ACK:** Graceful close.
- **ABORT:** Immediate close.
- **ERROR:** Error notification.

### Q9. What is the SCTP common header?
**Answer:** 12 bytes:
- Source Port (2 bytes)
- Destination Port (2 bytes)
- Verification Tag (4 bytes)
- Checksum (4 bytes, CRC32c)

Then chunks follow.

### Q10. What is multi-streaming in SCTP?
**Answer:** An association has N input streams and N output streams. Each stream has independent sequence. Messages on one stream don't block others. NGAP uses stream 0 for non-UE-associated, others for UE-associated.

### Q11. How to use SCTP in Linux?
**Answer:**
- Install: `sudo apt install libsctp-dev libsctp1 lksctp-tools`.
- Tools: `sctp_test`, `sctp_darn`, `withsctp`.
- C programming:
```c
int s = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
// Or SOCK_SEQPACKET for message-mode.
```

### Q12. What is the difference between one-to-one and one-to-many SCTP socket?
**Answer:**
- **One-to-one (SOCK_STREAM):** Like TCP. One socket = one association.
- **One-to-many (SOCK_SEQPACKET):** One socket = multiple associations. Server-friendly.

### Q13. How to capture SCTP with tcpdump?
**Answer:**
```bash
sudo tcpdump -i any 'sctp' -w sctp.pcap
sudo tcpdump -i any 'sctp port 38412' -w ngap.pcap
```

### Q14. How to view SCTP in Wireshark?
**Answer:** Open pcap. Filter: `sctp` or `ngap` or `f1ap`. Wireshark decodes NGAP/F1AP if SCTP port is set correctly. May need to right-click "Decode As" -> NGAP.

### Q15. What are common SCTP issues in deployment?
**Answer:**
- **Kernel module not loaded:** `modprobe sctp`.
- **Firewall blocking:** Allow SCTP in iptables.
- **MTU issues:** SCTP uses path MTU discovery.
- **NAT not supported:** SCTP doesn't traverse NAT well (use special NAT helper).

---

## SECTION B: GTP-U (16-30)

### Q16. What is GTP-U?
**Answer:** GTP-U (GPRS Tunneling Protocol - User plane) tunnels user data over IP. Carries IP-in-IP (or Ethernet-in-IP) between RAN and Core.

### Q17. Where is GTP-U used?
**Answer:**
- **N3:** gNB to UPF.
- **N9:** UPF to UPF (e.g., I-UPF to A-UPF, or roaming).
- **F1-U:** DU to CU-UP.
- **S1-U / S5-U (4G):** eNB to SGW to PGW.

### Q18. What is the GTP-U port?
**Answer:** UDP port **2152**.

### Q19. What is the GTP-U header structure?
**Answer:** Minimum 8 bytes:
```
+--------+--------+--------+--------+
| Flags  | MsgType|     Length      |  (4 bytes)
+--------+--------+--------+--------+
|             TEID                  |  (4 bytes)
+-----------------------------------+
```

Flags byte (bit 7 to 0):
- Version (3 bits): 001 for GTPv1.
- PT (1 bit): Protocol Type (1 = GTP, 0 = GTP').
- Reserved (1 bit).
- E (1 bit): Extension Header flag.
- S (1 bit): Sequence Number flag.
- PN (1 bit): N-PDU Number flag.

### Q20. What are GTP-U message types?
**Answer:**
- **0x01 Echo Request**
- **0x02 Echo Response**
- **0x1A Error Indication**
- **0x1F Supported Extension Headers**
- **0xFE End Marker** (path switch in handover)
- **0xFF G-PDU** (user data) - most common!

### Q21. What is TEID?
**Answer:** TEID (Tunnel Endpoint Identifier) is a 32-bit number that identifies a tunnel at the receiver. Each PDU session (and each direction) has a unique TEID.

### Q22. What is F-TEID?
**Answer:** F-TEID (Fully Qualified TEID) = TEID + IP address. Identifies one end of a tunnel uniquely across all nodes.

### Q23. What is the GTP-U PDU Session Container?
**Answer:** A GTP-U extension header carrying:
- **PDU Type** (DL or UL).
- **QFI** (QoS Flow Identifier).
- **RQI** (Reflective QoS Indicator).
- Other QoS info.

Used to convey QoS Flow info between gNB and UPF (and DU and CU-UP via F1-U).

### Q24. What is End Marker?
**Answer:** During handover, source gNB sends End Marker on each DL tunnel to indicate "no more data after this". Used to maintain in-order delivery.

### Q25. How is GTP-U encapsulation done in UPF?
**Answer:** When DL packet arrives:
1. UPF matches PDR (UE IP, optional filter).
2. Finds outer tunnel info (gNB IP, DL TEID, QFI).
3. Builds packet: [IP|UDP|GTP-U|PDU Session Container|inner IP packet].
4. Sends to gNB.

### Q26. How is GTP-U decapsulation done?
**Answer:** When UL packet arrives at UPF:
1. Receives UDP on port 2152.
2. Reads GTP-U header, extracts TEID.
3. Looks up PDR by TEID.
4. Strips GTP-U header.
5. Forwards inner packet to N6.

### Q27. How is GTP-U different from GTP-C?
**Answer:**
- **GTP-U:** User data. UDP 2152. Used in 4G and 5G.
- **GTP-C:** Control plane (4G). UDP 2123. Session management, mobility. NOT used in 5G core (replaced by HTTP/2 and PFCP).

### Q28. What is GTP-U Echo Request?
**Answer:** Keep-alive between GTP-U endpoints (e.g., gNB and UPF). Sent periodically. If no response, the peer is considered down.

### Q29. How can you debug GTP-U in Wireshark?
**Answer:**
- Filter: `gtp`.
- Expand "GPRS Tunneling Protocol" to see TEID, message type.
- Check inner packet (IP, then payload).
- Look for sequence numbers if S flag set.

### Q30. What is GTP-U sequence number?
**Answer:** Optional. When S flag = 1, header includes 2-byte sequence number. Used for in-order delivery (mostly in handover and HARQ).

---

## SECTION C: PFCP (31-40)

### Q31. What is PFCP?
**Answer:** PFCP (Packet Forwarding Control Protocol) - protocol between control plane (SMF) and user plane (UPF). On N4 interface. Specified in 3GPP TS 29.244. UDP port **8805**.

### Q32. Why was PFCP introduced?
**Answer:** To separate control and user plane (CUPS). Same protocol works for 4G (between SGW-C and SGW-U, etc.) and 5G (SMF to UPF).

### Q33. What are PFCP message types?
**Answer:**
- **Node-related:** Heartbeat, Association Setup/Update/Release, Node Report, PFD Management.
- **Session-related:** Session Establishment, Session Modification, Session Deletion, Session Report.

### Q34. What is PFCP Association?
**Answer:** First, SMF and UPF establish "association" (PFCP Association Setup Request/Response). This is the path-level binding. Then per-UE sessions are created.

### Q35. What is in PFCP Session Establishment Request?
**Answer:** SMF tells UPF:
- F-SEID (session ID).
- Create PDR (Packet Detection Rule): how to identify packet.
- Create FAR (Forwarding Action Rule): what action.
- Create QER (QoS Enforcement Rule).
- Create URR (Usage Reporting Rule).
- Create BAR (Buffering Action Rule).

### Q36. What is PDR?
**Answer:** PDR (Packet Detection Rule) tells UPF how to identify packets:
- **Source Interface:** Access (from gNB), Core (from N6), CP-function.
- **PDI (Packet Detection Info):** UE IP, F-TEID, SDF filter (5-tuple).
- **Precedence:** Order of matching.
- Associated FAR ID, QER ID, URR ID.

### Q37. What is FAR?
**Answer:** FAR (Forwarding Action Rule):
- **Apply Action:** FORW (forward), DROP, BUFF (buffer), NOCP (notify CP), DUPL (duplicate).
- **Forwarding Parameters:** Destination interface, network instance, outer header creation (for tunneling).

### Q38. What is QER?
**Answer:** QER (QoS Enforcement Rule):
- **Gate Status:** OPEN/CLOSED.
- **MBR:** Maximum Bit Rate (UL, DL).
- **GBR:** Guaranteed Bit Rate (UL, DL).
- **QFI:** QoS Flow ID.
- **Packet Rate Status:** packets per second.

### Q39. What is URR?
**Answer:** URR (Usage Reporting Rule):
- **Volume threshold:** Report when X bytes used.
- **Time threshold:** Report every X seconds.
- **Event Information:** What to report.

Used for charging.

### Q40. What is PFCP Session Report?
**Answer:** UPF reports to SMF:
- **DLDR (DL Data Report):** First downlink packet for IDLE UE (triggers paging).
- **USAR (Usage Report):** Volume/time reports for charging.
- **ERIR (Error Indication Report):** Tunnel error.

---

## SECTION D: N2/N3/F1 Integration (41-50)

### Q41. What does N2 setup look like in NGAP?
**Answer:**
1. SCTP association established (gNB <-> AMF).
2. gNB sends **NG Setup Request** (gNB ID, name, supported TAs, PLMN, slices).
3. AMF sends **NG Setup Response** (AMF name, GUAMI list, supported PLMN, AMF capacity).
4. Now ready for UE messages.

### Q42. What are the main NGAP messages?
**Answer:**
- **NG Setup:** Initial setup.
- **Initial UE Message:** First message from gNB for a UE.
- **DownlinkNasTransport / UplinkNasTransport:** NAS transport.
- **Initial Context Setup Request/Response:** Setup UE context (auth, sec, DRB).
- **PDU Session Resource Setup/Modify/Release:** Manage user plane.
- **UE Context Release:** Release UE context.
- **Handover:** N2 handover messages.
- **Paging.**

### Q43. What is "Initial Context Setup" in NGAP?
**Answer:** AMF sends this to gNB after authentication. Contains:
- Security key (K_gNB).
- Allowed NSSAI.
- UE capability.
- PDU session resources (for first session).
- NAS message (Registration Accept).

gNB sets up DRBs and replies with Initial Context Setup Response.

### Q44. What is N3 path establishment?
**Answer:**
1. SMF -> UPF: PFCP Session Establish (creates DL tunnel info, gets UL F-TEID).
2. SMF -> AMF: PDU Session Resource Setup (includes UL F-TEID + QFI list).
3. AMF -> gNB: NGAP PDU Session Resource Setup (with UL F-TEID).
4. gNB sets up DRB, creates DL F-TEID.
5. gNB -> AMF -> SMF: PDU Session Resource Setup Response (with DL F-TEID).
6. SMF -> UPF: PFCP Session Modify (with DL F-TEID).
7. Data flows on N3.

### Q45. How is F1 setup done?
**Answer:**
1. SCTP between DU and CU-CP.
2. DU sends **F1 Setup Request**: gNB-DU-ID, cells (NR Cell ID, PLMN, NSSAI, TAC, freq).
3. CU sends **F1 Setup Response**: gNB-CU name, cells to activate, RRC version.
4. CU sends **gNB-CU Configuration Update** if needed.

### Q46. What is gNB-DU and gNB-CU configuration update?
**Answer:**
- **gNB-DU Configuration Update:** DU notifies CU of cell additions/changes.
- **gNB-CU Configuration Update:** CU notifies DU of UE-associated config.

### Q47. What is UE Context Setup over F1?
**Answer:**
1. CU receives RRC Setup Complete (via Initial UL RRC Message Transfer from DU).
2. CU sends F1AP **UE Context Setup Request** to DU.
3. DU configures bearers, allocates resources, replies **UE Context Setup Response**.
4. DU sends RRC Reconfiguration to UE.

### Q48. How does QoS map across N3 -> F1-U -> Uu?
**Answer:**
- N3: GTP-U with QFI in PDU Session Container.
- gNB CU-UP: SDAP maps QFI to DRB.
- F1-U: GTP-U with QFI (CU-UP to DU).
- DU: Schedules data on DRB at MAC layer with QoS profile.
- Air: PDCP/RLC/MAC on configured DRB.

### Q49. What is the typical packet flow from UE to internet?
**Answer:**
```
UE -> Uu -> [PHY/MAC/RLC/PDCP/SDAP] gNB-DU 
   -> F1-U (GTP-U) -> gNB-CU-UP 
   -> N3 (GTP-U) -> UPF 
   -> N6 (IP) -> Internet
```

### Q50. What if N3 tunnel breaks?
**Answer:** Data is lost. UPF may buffer (BAR + FAR=BUFF). gNB may detect via GTP-U Echo failure. Session is released (PFCP Session Deletion). UE re-establishes.

---

## SECTION E: NAS, RRC, F1AP Protocol Details (51-60)

### Q51. What is NAS protocol stack?
**Answer:**
```
NAS (5GMM, 5GSM)
  |
[over RRC over Uu], [over N2 NGAP between gNB and AMF]
```
NAS is end-to-end between UE and AMF.

### Q52. What are 5GMM and 5GSM?
**Answer:**
- **5GMM (5G Mobility Management):** Registration, deregistration, identification, security mode, authentication, configuration update, paging.
- **5GSM (5G Session Management):** PDU session establish/modify/release, PDU session authentication.

### Q53. How is NAS protected?
**Answer:** After security mode command:
- **Integrity:** Always on (mandatory).
- **Ciphering:** Optional.

Uses K_NASint and K_NASenc derived from K_AMF.

### Q54. What are NAS message types?
**Answer:** Many. Examples:
- 5GMM: Registration Request (0x41), Registration Accept (0x42), Authentication Request (0x56), Security Mode Command (0x5D), Service Request (0x4C).
- 5GSM: PDU Session Establishment Request (0xC1), Accept (0xC2), Reject (0xC3).

### Q55. What is the RRC protocol stack?
**Answer:** RRC PDUs go on:
- **SRB0:** RRC Setup Request, RRC Setup. CCCH.
- **SRB1:** Most RRC messages. DCCH.
- **SRB2:** NAS messages after security activated.
- **SRB3:** For dual connectivity (secondary gNB).

### Q56. What is the difference between SRB and DRB?
**Answer:**
- **SRB (Signaling Radio Bearer):** Carries control signaling (RRC, NAS).
- **DRB (Data Radio Bearer):** Carries user data.

### Q57. What is the PDCP role in security?
**Answer:** PDCP applies:
- **Integrity protection:** SRBs always, DRBs if configured.
- **Ciphering:** SRBs (except SRB0) and DRBs if configured.

Uses K_RRCint, K_RRCenc, K_UPint, K_UPenc.

### Q58. What is RLC mode for SRB and DRB?
**Answer:**
- **SRB:** Always AM (Acknowledged Mode) for reliability.
- **DRB:** UM or AM depending on service:
  - UM for voice (real-time, can lose packets).
  - AM for data (reliable).

### Q59. What is F1AP UE association?
**Answer:** Each UE has:
- **gNB-CU UE F1AP ID** (CU side).
- **gNB-DU UE F1AP ID** (DU side).

These are included in F1AP messages to associate them with the right UE.

### Q60. What is the typical bootup sequence of a gNB?
**Answer:**
1. DU starts, loads cell config.
2. DU connects to CU-CP over SCTP, sends F1 Setup Request.
3. CU-CP responds. F1 established.
4. CU-CP connects to AMF over SCTP, sends NG Setup Request.
5. AMF responds. N2 established.
6. CU-CP <-> CU-UP: E1 Setup.
7. SCTP/HTTP/2 to NRF, SMF discovery.
8. Cells active, ready for UEs.

---

## END OF LEVEL 5 (60 questions)

> Move to **06_LINUX_DOCKER_TOOLS.md**.
