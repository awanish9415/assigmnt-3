# Quick Revision Sheet (Read on Wednesday Morning)

> One-page cheat sheet. Read for 30 minutes before interview.

---

## 🎯 TOP 30 MUST-KNOW ANSWERS

### 1. 5G Architecture in 30 seconds
```
UE <--Uu--> gNB <--N2/NGAP--> AMF <--HTTP/2--> SMF <--PFCP/N4--> UPF
                                                                   |
                                                                   N6 -> Internet
```

### 2. Main 5GC NFs
**AMF** (mobility), **SMF** (session), **UPF** (data forwarding), **PCF** (policy), **UDM** (subscribers), **AUSF** (auth), **NRF** (discovery), **NSSF** (slice select).

### 3. Key Interfaces & Protocols
| Interface | What | Protocol |
|-----------|------|----------|
| N1 | UE↔AMF | NAS |
| N2 | gNB↔AMF | NGAP/SCTP:38412 |
| N3 | gNB↔UPF | GTP-U/UDP:2152 |
| N4 | SMF↔UPF | PFCP/UDP:8805 |
| F1-C | DU↔CU | F1AP/SCTP:38472 |
| F1-U | DU↔CU-UP | GTP-U |
| E1 | CU-CP↔CU-UP | E1AP/SCTP:38462 |
| OFH | DU↔RU | eCPRI |
| SBA | NF↔NF | HTTP/2 |

### 4. gNB Split (O-RAN)
- **O-RU:** Low PHY, RF.
- **O-DU:** RLC, MAC, High PHY.
- **O-CU:** PDCP, SDAP, RRC.
- **CU-CP:** Control plane.
- **CU-UP:** User plane.

### 5. 3GPP Split Options
- **Option 2:** PDCP/RLC (CU/DU). F1.
- **Option 7.2x:** Inside PHY (DU/RU). Open Fronthaul.
- **Option 8:** PHY/RF (CPRI).

### 6. Protocol Stack (Bottom to Top)
PHY → MAC → RLC → PDCP → SDAP (5G) → RRC (CP)/IP (UP)

### 7. SCTP Key Points
- Multi-streaming + multi-homing.
- Message-oriented.
- 4-way handshake (with cookie).
- Used: NGAP (38412), F1AP (38472), E1AP (38462).

### 8. GTP-U Key Points
- UDP port 2152.
- 8-byte header: flags, msg_type, length, TEID.
- Carries inner IP packet.
- PDU Session Container = extension header with QFI.

### 9. PFCP Key Points
- N4: SMF↔UPF.
- UDP 8805.
- Rules: **PDR** (detect), **FAR** (action), **QER** (QoS), **URR** (usage), **BAR** (buffer).

### 10. QoS Hierarchy in 5G
**PDU Session** → contains multiple **QoS Flows** (each has QFI + 5QI) → mapped to **DRBs** (by SDAP) → carried on **Uu**.

### 11. 5QI Values to Remember
- **1:** Voice (GBR, 100ms, prio 20).
- **9:** Default internet (Non-GBR).
- **82:** Discrete automation (DC-GBR, 10ms, 255 byte burst).
- **83:** Discrete automation (DC-GBR, 10ms, 1354 byte burst).
- **DC-GBR range:** 82-87.

### 12. 5G Identifiers
- **SUPI:** Permanent ID (IMSI-like).
- **SUCI:** Encrypted SUPI.
- **5G-GUTI:** Temporary ID.
- **PEI:** Equipment ID (IMEI).

### 13. NSSAI / Slicing
- **S-NSSAI = SST + SD**.
- **SST 1** = eMBB, **2** = URLLC, **3** = mMTC, **4** = V2X.
- AMF gives Allowed NSSAI based on subscription + NSSF.

### 14. RRC States in 5G
- **IDLE:** Not connected.
- **INACTIVE:** Context kept, fast resume.
- **CONNECTED:** Active.

### 15. Registration Flow (Brief)
1. RRC Setup (UE↔gNB).
2. NAS Registration Request to AMF.
3. AKA auth via AUSF/UDM.
4. NAS Security setup.
5. Registration Accept.

### 16. PDU Session Establishment (Brief)
1. UE → AMF: PDU Session Establish Request.
2. AMF → SMF (N11).
3. SMF → UPF: PFCP Session Establish.
4. SMF → AMF → gNB: NGAP PDU Resource Setup.
5. gNB sets up DRB, replies.
6. Data flows.

### 17. Open5GS Quick Facts
- C, Meson+Ninja, MongoDB.
- WebUI on port 9999.
- Each NF has YAML config in `/etc/open5gs/`.
- TUN device `ogstun` for UPF.
- 10.45.0.0/16 default UE subnet.

### 18. srsRAN Quick Facts
- C++, CMake.
- srsRAN Project = 5G SA gNB only.
- ZMQ for software UE (no radio).
- Config: gnb.yml (YAML).
- O-RAN 7.2 support via DPDK.

### 19. UPF Data Flow (Open5GS)
UE → gNB → GTP-U on N3 → UPF receives on UDP 2152 → decapsulates → writes to ogstun → kernel routes via iptables NAT → eth0 → Internet.

### 20. PTP / Timing
- **G.8275.1:** Full timing support, multicast L2.
- **G.8275.2:** Partial, unicast L3.
- **ptp4l:** PTP daemon (syncs PHC to GM).
- **phc2sys:** Syncs sys clock to PHC.
- **Hw timestamping** needed (check with `ethtool -T`).

### 21. Network Order Conversion in C
```c
htonl(), htons()  // host to network
ntohl(), ntohs()  // network to host
```

### 22. Common Linux Commands
```bash
ip addr show
ip route show
tcpdump -i any -w out.pcap
ethtool -T eth0
sctp_test
ss -anp | grep sctp
systemctl status open5gs-amfd
journalctl -u open5gs-smfd
```

### 23. iptables NAT for UPF
```bash
sudo iptables -t nat -A POSTROUTING -s 10.45.0.0/16 ! -o ogstun -j MASQUERADE
sudo sysctl -w net.ipv4.ip_forward=1
```

### 24. SCTP in C
```c
int s = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
// Or SOCK_SEQPACKET for one-to-many
```

### 25. GTP-U Header Parsing (Mental Model)
- Byte 0: flags (version|PT|R|E|S|PN).
- Byte 1: message type (0xFF = G-PDU).
- Bytes 2-3: length (big-endian).
- Bytes 4-7: TEID (big-endian).
- Then optional ext, then inner IP.

### 26. Build Commands
```bash
# CMake (srsRAN)
mkdir build && cd build && cmake .. && make -j$(nproc)

# Meson (Open5GS)
meson build && ninja -C build && ninja -C build install
```

### 27. Debugging Tools
- **gdb:** binary debug.
- **Valgrind:** memory.
- **ASAN:** `-fsanitize=address`.
- **perf:** profiling.
- **strace:** syscalls.
- **Wireshark/tcpdump:** packets.

### 28. Common Issues Cheat
| Symptom | Likely Cause |
|---------|-------------|
| NG Setup fails | PLMN/TAC/Slice mismatch |
| UE register OK, PDU fail | DNN/SMF/UPF issue |
| Data forwarding fails | NAT/ogstun/IP forward |
| Random crashes under load | Race condition / OOM |
| High latency | CPU pinning, scheduling |

### 29. DC-GBR Selling Points
- Low PDB (5-30 ms).
- Guaranteed bit rate.
- Max Data Burst Volume parameter.
- Use cases: URLLC, factory automation, V2X.

### 30. Top Telecom Acronyms Quick Lookup
| Acronym | Full |
|---------|------|
| AMF | Access and Mobility Management Function |
| SMF | Session Management Function |
| UPF | User Plane Function |
| PCF | Policy Control Function |
| UDM | Unified Data Management |
| AUSF | Authentication Server Function |
| NRF | NF Repository Function |
| NSSF | Network Slice Selection Function |
| gNB | next-generation NodeB |
| CU | Central Unit |
| DU | Distributed Unit |
| RU | Radio Unit |
| OFH | Open Fronthaul |
| eCPRI | enhanced CPRI |
| SBA | Service Based Architecture |
| QFI | QoS Flow Identifier |
| 5QI | 5G QoS Identifier |
| DRB | Data Radio Bearer |
| S-NSSAI | Single Network Slice Selection Assistance Info |
| SST | Slice/Service Type |
| DNN | Data Network Name |
| GUTI | Globally Unique Temporary Identifier |
| SUPI | Subscription Permanent Identifier |
| SUCI | Subscription Concealed Identifier |
| TEID | Tunnel Endpoint Identifier |
| PFCP | Packet Forwarding Control Protocol |
| NGAP | NG Application Protocol |
| F1AP | F1 Application Protocol |
| RIC | RAN Intelligent Controller |
| SMO | Service Management and Orchestration |

---

## 🎤 INTERVIEW DAY CHECKLIST

### Night before:
- [ ] Sleep 7-8 hours.
- [ ] Test Teams meeting link.
- [ ] Test mic, camera, headphones.
- [ ] Charge laptop fully.
- [ ] Keep notebook + pen ready.
- [ ] Keep water glass ready.

### Morning of interview:
- [ ] Light breakfast.
- [ ] Read this revision sheet once.
- [ ] Read Master Index.
- [ ] Look at JD again — focus on top skills.
- [ ] Be online 10 min before.
- [ ] Wear formal/semi-formal shirt.
- [ ] Good lighting (face well-lit).
- [ ] Quiet room.

### During interview:
- [ ] Smile and greet.
- [ ] Listen carefully — don't interrupt.
- [ ] Take 2-3 seconds before answering complex Qs.
- [ ] If unsure: "I have not worked on this directly. My understanding is..."
- [ ] Use diagrams if allowed (whiteboard / paper).
- [ ] Be honest about gaps.
- [ ] Ask thoughtful questions at end.
- [ ] Thank interviewer.

### Common phrases that work:
- "That's a good question. Let me think for a moment."
- "Based on my understanding..."
- "In my previous project, we used a similar approach where..."
- "I am not 100% sure, but I believe..."
- "Could you clarify what you mean by [X]?"

---

## 🚀 YOU GOT THIS! BEST OF LUCK! 🚀

Remember: They are looking for a thoughtful engineer who can learn, not a walking encyclopedia. Be calm, be honest, be curious.

If you don't know something, that's OK — say so AND say what you would do to find out.
