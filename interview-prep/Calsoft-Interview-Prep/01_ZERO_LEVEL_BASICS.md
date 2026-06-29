# Level 1: Zero-Level Basics (Networking + Telecom + 5G Fundamentals)

> Start here even if you know basics. Interviewer often starts with simple questions to test foundation.

---

## SECTION A: Networking Basics (1-25)

### Q1. What is an IP address?
**Answer:** IP address is a unique number given to every device on a network. It is like a home address. Two types: IPv4 (32-bit, like 192.168.1.1) and IPv6 (128-bit).

### Q2. What is the difference between TCP and UDP?
**Answer:**
- **TCP:** Connection-based, reliable, ordered delivery, slower. Used for web, email, file transfer.
- **UDP:** Connectionless, fast, no guarantee of delivery. Used for video calls, streaming, DNS, GTP-U.

### Q3. What is a MAC address?
**Answer:** MAC (Media Access Control) is a 48-bit hardware address of a network card. It is fixed by the manufacturer. Example: `aa:bb:cc:11:22:33`.

### Q4. What is the OSI model? Name all 7 layers.
**Answer:**
1. **Physical** - cables, signals
2. **Data Link** - MAC, Ethernet, frames
3. **Network** - IP, routing
4. **Transport** - TCP, UDP
5. **Session** - manages connection
6. **Presentation** - encryption, encoding
7. **Application** - HTTP, FTP, DNS

### Q5. What is TCP/IP model?
**Answer:** Simpler model with 4 layers:
1. Link (Ethernet, Wi-Fi)
2. Internet (IP)
3. Transport (TCP, UDP)
4. Application (HTTP, DNS, SSH)

### Q6. What is the difference between hub, switch and router?
**Answer:**
- **Hub:** Sends data to all ports (dumb device, Layer 1).
- **Switch:** Sends data only to the correct port using MAC address (Layer 2).
- **Router:** Sends data between different networks using IP address (Layer 3).

### Q7. What is a subnet mask?
**Answer:** Subnet mask separates IP address into network part and host part. Example: `255.255.255.0` means first 24 bits are network, last 8 bits are host.

### Q8. What is the difference between public and private IP?
**Answer:**
- **Private IP:** Used inside home/office (10.x.x.x, 172.16-31.x.x, 192.168.x.x). Not visible on internet.
- **Public IP:** Used on internet, visible to all.

### Q9. What is NAT?
**Answer:** NAT (Network Address Translation) converts private IP to public IP. Allows many devices to share one public IP.

### Q10. What is DNS?
**Answer:** DNS (Domain Name System) converts website name (like google.com) to IP address. It is like a phone book of internet.

### Q11. What is DHCP?
**Answer:** DHCP gives IP address automatically to devices when they connect to network. No manual IP setting needed.

### Q12. What is a port number?
**Answer:** Port number identifies an application on a device. Range 0-65535. Examples:
- HTTP - 80
- HTTPS - 443
- SSH - 22
- DNS - 53
- SCTP NGAP - 38412
- GTP-U - 2152

### Q13. What is the difference between IPv4 and IPv6?
**Answer:**
- **IPv4:** 32 bits, 4.3 billion addresses, format `192.168.1.1`
- **IPv6:** 128 bits, almost unlimited addresses, format `2001:db8::1`

### Q14. What is ARP?
**Answer:** ARP (Address Resolution Protocol) finds MAC address from IP address in local network.

### Q15. What is ICMP?
**Answer:** ICMP is used for sending error messages and diagnostic info. `ping` command uses ICMP.

### Q16. What is the difference between latency, jitter and throughput?
**Answer:**
- **Latency:** Time taken for a packet to travel (one-way or round-trip).
- **Jitter:** Variation in latency over time.
- **Throughput:** Actual data transferred per second (bits/sec).

### Q17. What is bandwidth?
**Answer:** Maximum capacity of a network link. Measured in bits per second (bps, Kbps, Mbps, Gbps).

### Q18. What is a VPN?
**Answer:** VPN (Virtual Private Network) creates a secure encrypted tunnel between two networks over internet.

### Q19. What is a firewall?
**Answer:** Firewall is a security system that controls incoming and outgoing network traffic based on rules.

### Q20. What is a MTU?
**Answer:** MTU (Maximum Transmission Unit) is the largest size of a packet that can be sent without fragmentation. Ethernet default MTU is 1500 bytes.

### Q21. What is fragmentation?
**Answer:** If a packet is bigger than MTU, it is broken into smaller pieces. This is called fragmentation.

### Q22. What is the 3-way handshake in TCP?
**Answer:**
1. Client sends **SYN** to server.
2. Server replies with **SYN-ACK**.
3. Client sends **ACK**. Connection established.

### Q23. What is the difference between half-duplex and full-duplex?
**Answer:**
- **Half-duplex:** Data flows in one direction at a time (walkie-talkie).
- **Full-duplex:** Data flows in both directions simultaneously (phone call).

### Q24. What is VLAN?
**Answer:** VLAN (Virtual LAN) divides a physical network into multiple logical networks. Tagged using 802.1Q (4-byte tag).

### Q25. What is the difference between unicast, broadcast and multicast?
**Answer:**
- **Unicast:** One-to-one (most common).
- **Broadcast:** One-to-all in a network.
- **Multicast:** One-to-group (selected devices).

---

## SECTION B: Telecom Generations (26-40)

### Q26. Tell me about evolution from 1G to 5G.
**Answer:**
- **1G:** Analog voice (1980s)
- **2G (GSM):** Digital voice + SMS (1990s)
- **3G (UMTS):** Mobile internet (2000s)
- **4G (LTE):** High-speed data, all-IP (2010s)
- **5G (NR):** Ultra-fast, low latency, IoT (2020s)

### Q27. What is the difference between 4G and 5G?
**Answer:**
| Feature | 4G | 5G |
|---------|-----|-----|
| Speed | 100 Mbps | 10 Gbps |
| Latency | 30-50 ms | 1-10 ms |
| Frequency | Below 6 GHz | Sub-6 + mmWave |
| Architecture | EPC | 5GC (SBA) |
| Use Cases | Mobile broadband | eMBB, URLLC, mMTC |

### Q28. What is 5G NR?
**Answer:** 5G NR (New Radio) is the new radio interface for 5G. It works in two frequency ranges:
- **FR1:** below 6 GHz (sub-6)
- **FR2:** 24-52 GHz (mmWave)

### Q29. What are the three main use cases of 5G?
**Answer:**
1. **eMBB** (enhanced Mobile Broadband) - High-speed internet, 4K/8K video.
2. **URLLC** (Ultra-Reliable Low-Latency Communication) - Self-driving cars, remote surgery.
3. **mMTC** (massive Machine-Type Communication) - IoT, billions of devices.

### Q30. What is 5G SA and NSA?
**Answer:**
- **NSA (Non-Standalone):** 5G radio with 4G core (EPC). Used for early 5G deployment.
- **SA (Standalone):** 5G radio with 5G core (5GC). True 5G with all features.

### Q31. What is the difference between EPC and 5GC?
**Answer:**
- **EPC (4G Core):** MME, SGW, PGW, HSS. Uses GTP, Diameter.
- **5GC (5G Core):** AMF, SMF, UPF, PCF, UDM, AUSF, NRF. Uses HTTP/2 (SBA), GTP-U for user plane.

### Q32. What is gNB?
**Answer:** gNB is the 5G base station (next-generation NodeB). It connects User Equipment (UE) to 5G core.

### Q33. What is eNB?
**Answer:** eNB is the 4G/LTE base station (evolved NodeB).

### Q34. What is en-gNB?
**Answer:** en-gNB is a 5G base station that connects to 4G core (EPC) in NSA mode.

### Q35. What is UE?
**Answer:** UE (User Equipment) is the mobile phone, tablet, IoT device, etc.

### Q36. What is the air interface in 5G?
**Answer:** Uu interface - between UE and gNB (radio link).

### Q37. What is the difference between FDD and TDD?
**Answer:**
- **FDD (Frequency Division Duplex):** Uplink and downlink on different frequencies.
- **TDD (Time Division Duplex):** Uplink and downlink on same frequency at different times. Used in 5G.

### Q38. What is OFDM?
**Answer:** OFDM (Orthogonal Frequency Division Multiplexing) divides a wide channel into many small narrow subcarriers. Used in 4G and 5G.

### Q39. What is the subcarrier spacing in 5G?
**Answer:** 5G uses flexible numerology: 15, 30, 60, 120, 240 kHz. Higher spacing = lower latency. (4G uses fixed 15 kHz.)

### Q40. What is beamforming?
**Answer:** Beamforming focuses radio signal in a specific direction (like a flashlight) instead of spreading in all directions. Improves coverage and capacity.

---

## SECTION C: Protocol Stack Basics (41-60)

### Q41. What are the layers in 5G protocol stack?
**Answer:** From bottom to top:
- **PHY** (Physical layer)
- **MAC** (Medium Access Control)
- **RLC** (Radio Link Control)
- **PDCP** (Packet Data Convergence Protocol)
- **SDAP** (Service Data Adaptation Protocol) - new in 5G
- **RRC** (Radio Resource Control) - control plane only
- **NAS** (Non-Access Stratum) - between UE and AMF

### Q42. What does PHY layer do?
**Answer:** Physical layer handles raw radio signals: modulation, coding, mapping to antennas, beamforming.

### Q43. What does MAC layer do?
**Answer:**
- Maps logical channels to transport channels
- Scheduling (which UE gets which resources)
- HARQ (Hybrid ARQ for retransmission)
- Random access
- Multiplexing and demultiplexing

### Q44. What does RLC layer do?
**Answer:** RLC handles:
- Segmentation and reassembly
- ARQ (retransmission)
- Three modes: TM (Transparent), UM (Unacknowledged), AM (Acknowledged)

### Q45. What does PDCP layer do?
**Answer:** PDCP handles:
- Header compression (ROHC)
- Ciphering (encryption)
- Integrity protection
- Sequence numbering
- Duplicate detection

### Q46. What does SDAP layer do?
**Answer:** SDAP maps QoS flows from core to Data Radio Bearers (DRB) in the radio. It is new in 5G (not in 4G).

### Q47. What does RRC layer do?
**Answer:** RRC is the control plane between UE and gNB. Handles connection setup, mobility, measurements, security activation.

### Q48. What is NAS?
**Answer:** NAS (Non-Access Stratum) is the control plane between UE and AMF (in 5G core). Handles registration, session management.

### Q49. What is the difference between AS and NAS?
**Answer:**
- **AS (Access Stratum):** Between UE and gNB. Includes PHY, MAC, RLC, PDCP, RRC.
- **NAS (Non-Access Stratum):** Between UE and AMF. Goes through gNB transparently.

### Q50. What is control plane and user plane?
**Answer:**
- **Control plane (C-plane):** Signaling messages (registration, session setup, mobility).
- **User plane (U-plane):** Actual user data (web browsing, video, voice).

### Q51. What is logical channel?
**Answer:** Logical channel is the "what" of data - the type of information being transferred (e.g., DCCH for control, DTCH for data).

### Q52. What is transport channel?
**Answer:** Transport channel is the "how" of data - how the data is transmitted over the air (e.g., DL-SCH, UL-SCH).

### Q53. What is physical channel?
**Answer:** Physical channel carries actual bits over the air (e.g., PDSCH, PUSCH, PDCCH, PUCCH).

### Q54. Name some downlink physical channels in 5G.
**Answer:**
- **PDSCH** - Physical Downlink Shared Channel (user data)
- **PDCCH** - Physical Downlink Control Channel (scheduling info)
- **PBCH** - Physical Broadcast Channel (system info)
- **PSS/SSS** - Primary/Secondary Synchronization Signal

### Q55. Name some uplink physical channels in 5G.
**Answer:**
- **PUSCH** - Physical Uplink Shared Channel
- **PUCCH** - Physical Uplink Control Channel
- **PRACH** - Physical Random Access Channel

### Q56. What is HARQ?
**Answer:** HARQ (Hybrid ARQ) combines FEC and ARQ. If a packet is corrupt, receiver asks for retransmission and combines old + new for decoding. Done at MAC layer.

### Q57. What is the difference between ARQ and HARQ?
**Answer:**
- **ARQ:** Pure retransmission (RLC).
- **HARQ:** Retransmission + soft combining (MAC layer). Faster.

### Q58. What is a slot in 5G?
**Answer:** Time unit in 5G. A slot has 14 OFDM symbols. Slot duration depends on numerology:
- 15 kHz → 1 ms
- 30 kHz → 0.5 ms
- 60 kHz → 0.25 ms

### Q59. What is a frame and subframe in 5G?
**Answer:**
- **Frame:** 10 ms
- **Subframe:** 1 ms (10 subframes per frame)
- **Slot:** varies by numerology

### Q60. What is Resource Block (RB) in 5G?
**Answer:** Resource Block = 12 subcarriers in frequency. It is the smallest unit of resource allocation.

---

## SECTION D: Common Terms (61-80)

### Q61. What is IMSI?
**Answer:** IMSI (International Mobile Subscriber Identity) is a unique number on SIM card. In 5G, it is replaced by SUPI.

### Q62. What is SUPI and SUCI?
**Answer:**
- **SUPI:** Subscription Permanent Identifier (long-term ID).
- **SUCI:** Subscription Concealed Identifier (encrypted SUPI for privacy).

### Q63. What is GUTI?
**Answer:** GUTI (Globally Unique Temporary Identifier) is a temporary ID given to UE after registration. Hides permanent identity.

### Q64. What is PLMN?
**Answer:** PLMN (Public Land Mobile Network) is the operator's network. Identified by MCC + MNC.
- **MCC:** Mobile Country Code (e.g., 404 = India)
- **MNC:** Mobile Network Code (operator ID)

### Q65. What is TAC and TAI?
**Answer:**
- **TAC:** Tracking Area Code (area where UE is tracked).
- **TAI:** PLMN + TAC.

### Q66. What is cell ID?
**Answer:** Unique number for each cell (sector) in the network.

### Q67. What is a PDU session?
**Answer:** PDU (Protocol Data Unit) session is a logical data connection between UE and 5G core. Like an APN/PDN in 4G.

### Q68. What is a bearer?
**Answer:** Bearer is a virtual pipe for carrying user data with specific QoS. In 5G, replaced by QoS flows + DRB.

### Q69. What is the difference between 4G bearer and 5G QoS flow?
**Answer:**
- **4G:** EPS bearer (default + dedicated), QCI per bearer.
- **5G:** QoS flow (more granular), 5QI per flow, mapped to DRB by SDAP.

### Q70. What is DRB?
**Answer:** DRB (Data Radio Bearer) carries user data over the radio between UE and gNB.

### Q71. What is SRB?
**Answer:** SRB (Signaling Radio Bearer) carries RRC and NAS signaling. SRB0, SRB1, SRB2, SRB3.

### Q72. What is QoS?
**Answer:** QoS (Quality of Service) means giving different priority/treatment to different traffic. Video call needs low latency, file download needs high throughput.

### Q73. What is 5QI?
**Answer:** 5QI (5G QoS Identifier) is a number that defines QoS characteristics like priority, delay, packet loss. Example: 5QI=1 for voice, 5QI=82/83 for low latency.

### Q74. What is GBR and Non-GBR?
**Answer:**
- **GBR (Guaranteed Bit Rate):** Bandwidth is reserved (voice, video call).
- **Non-GBR:** Best effort, no guarantee (web, email).

### Q75. What is DC-GBR?
**Answer:** DC-GBR (Delay Critical GBR) is a special GBR for ultra-low latency. Used for URLLC. 5QI 82/83 are DC-GBR.

### Q76. What is APN?
**Answer:** APN (Access Point Name) in 4G - tells which gateway/internet to connect to. In 5G, it is DNN.

### Q77. What is DNN?
**Answer:** DNN (Data Network Name) in 5G. Same purpose as APN. Example: "internet", "ims", "enterprise".

### Q78. What is Network Slicing?
**Answer:** Network Slicing creates multiple virtual networks on one physical 5G network. Each slice has different QoS for different use case (eMBB, URLLC, mMTC).

### Q79. What is NSSAI?
**Answer:** NSSAI (Network Slice Selection Assistance Information) identifies network slices. Contains S-NSSAI.
- **S-NSSAI:** Single NSSAI = SST (slice type) + SD (slice differentiator).

### Q80. What is SST values?
**Answer:**
- **SST 1:** eMBB
- **SST 2:** URLLC
- **SST 3:** mMTC
- **SST 4:** V2X

---

## END OF LEVEL 1 (80 questions)

> Move to **02_5G_ARCHITECTURE.md** next.
