# Level 4: Open5GS and 5G Core NFs (Senior Core Developer)

> Direct match to JD: "Open5GS deployment & config, 5G SA core NFs (AMF/SMF/UPF/PCF/NRF/UDM/AUSF), Ethernet PDU session, Network slicing/NSSAI/DNN, QoS policy (DC-GBR flows)"

---

## SECTION A: Open5GS Basics (1-20)

### Q1. What is Open5GS?
**Answer:** Open5GS is an open-source implementation of 5G Core (and 4G EPC). Written in C. Supports:
- 5G SA: AMF, SMF, UPF, PCF, UDM, UDR, AUSF, NRF, NSSF, BSF, SCP
- 4G EPC: MME, HSS, PCRF, SGW, PGW
- IMS: P-CSCF, I-CSCF, S-CSCF (limited)

### Q2. Who maintains Open5GS?
**Answer:** Sukchan Lee is the lead maintainer. Active community on GitHub. Open source under AGPL.

### Q3. What is the build system of Open5GS?
**Answer:** **Meson + Ninja**. Steps:
```bash
meson build --prefix=`pwd`/install
ninja -C build
ninja -C build install
```

### Q4. What database does Open5GS use?
**Answer:** **MongoDB** for subscriber data. Uses official MongoDB C driver. WebUI uses MongoDB for UE provisioning.

### Q5. What are config files in Open5GS?
**Answer:** YAML config files in `install/etc/open5gs/`:
- `amf.yaml`, `smf.yaml`, `upf.yaml`, `pcf.yaml`, `udm.yaml`, etc.
Each NF has its own YAML.

### Q6. How to install Open5GS on Ubuntu 22.04?
**Answer:**
```bash
sudo apt install software-properties-common
sudo add-apt-repository ppa:open5gs/latest
sudo apt update
sudo apt install open5gs
```
Or build from source with meson.

### Q7. How to install Open5GS WebUI?
**Answer:**
```bash
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt install -y nodejs
curl -fsSL https://open5gs.org/open5gs/assets/webui/install | sudo -E bash -
```
WebUI runs on port 9999.

### Q8. How do you provision a UE in Open5GS?
**Answer:** Via WebUI (port 9999):
1. Login (default admin/1423).
2. Subscriber > Add.
3. Enter IMSI, K (key), OPC (or OP), AMF (16 bits).
4. Set APN/DNN, slice (S-NSSAI), QoS (5QI, ARP).
5. Save.

### Q9. What is the default PLMN in Open5GS?
**Answer:** MCC=001, MNC=01 (test PLMN). Configure in each NF's YAML under `plmn_support`.

### Q10. How does Open5GS UPF do data forwarding?
**Answer:** Uses Linux TUN/TAP interface (`ogstun`). Default subnet: 10.45.0.0/16 (IPv4). UPF receives GTP-U from gNB, decapsulates, sends to `ogstun`. Returns reverse direction.

### Q11. How to configure UPF for data network?
**Answer:** In `upf.yaml`:
```yaml
upf:
  pfcp:
    server:
      - address: 127.0.0.7
  gtpu:
    server:
      - address: 127.0.0.7
  session:
    - subnet: 10.45.0.1/16
      dnn: internet
```
Plus, create `ogstun` interface:
```bash
sudo ip tuntap add name ogstun mode tun
sudo ip addr add 10.45.0.1/16 dev ogstun
sudo ip link set ogstun up
sudo iptables -t nat -A POSTROUTING -s 10.45.0.0/16 -o eth0 -j MASQUERADE
```

### Q12. How does control plane flow work in Open5GS?
**Answer:** AMF connects to gNB via NGAP/SCTP. NAS messages are passed to:
- MM messages: AMF
- SM messages: AMF -> SMF (via N11/HTTP2)

SMF -> UPF via PFCP (N4). SMF tells UPF which rules to apply.

### Q13. What is NRF in Open5GS?
**Answer:** NRF (Network Repository Function) is the "service discovery" of 5G. All NFs register with NRF. Other NFs query NRF to find each other.

### Q14. How does NF registration work?
**Answer:**
1. NF (e.g., AMF) starts.
2. AMF sends `NFRegister` to NRF (HTTP/2 PUT /nf-instances/{nfId}).
3. NRF stores AMF info.
4. AMF periodically sends `NFHeartbeat`.
5. To find SMF, AMF asks NRF: `NFDiscover` with NF type=SMF.

### Q15. What is SCP in Open5GS?
**Answer:** SCP (Service Communication Proxy) is a proxy for all HTTP/2 messages between NFs. Two modes:
- **Indirect via SCP:** All NF traffic goes through SCP (model D).
- **Direct without SCP:** NFs talk directly (model A/B/C).

Open5GS supports both.

### Q16. What is BSF in Open5GS?
**Answer:** BSF (Binding Support Function) - tracks which PCF handles which UE session. Used in roaming/multi-PCF deployments.

### Q17. How to check if all NFs are running?
**Answer:**
```bash
systemctl status open5gs-amfd open5gs-smfd open5gs-upfd
systemctl status open5gs-pcfd open5gs-udmd open5gs-udrd
systemctl status open5gs-ausfd open5gs-nrfd open5gs-nssfd
```
Or process check: `ps aux | grep open5gs`.

### Q18. Where are Open5GS logs?
**Answer:** `/var/log/open5gs/`:
- `amf.log`, `smf.log`, `upf.log`, etc.

### Q19. How to enable debug logs?
**Answer:** In each NF YAML:
```yaml
logger:
  file:
    path: /var/log/open5gs/amf.log
  level: debug
```

### Q20. How to test 5G UE registration without real RAN?
**Answer:** Use **UERANSIM** - software UE+gNB simulator. Install UERANSIM:
1. Build UERANSIM.
2. Configure `gnb.yaml` (AMF address, PLMN).
3. Configure `ue.yaml` (IMSI, key, OPC matching Open5GS subscriber).
4. Run `./nr-gnb -c gnb.yaml`.
5. Run `./nr-ue -c ue.yaml`.

---

## SECTION B: AMF Deep Dive (21-30)

### Q21. What protocols does AMF use?
**Answer:**
- **NGAP/SCTP** on N2 (with gNB).
- **HTTP/2** on N11 (with SMF), N12 (with AUSF), N8 (with UDM), N15 (with PCF), Nnrf with NRF.
- **NAS over RRC/N1** with UE.

### Q22. What are the main AMF procedures?
**Answer:**
- Registration / Deregistration
- Authentication (via AUSF)
- Security setup (NAS)
- Service Request
- PDU Session forwarding to SMF
- Handover (Xn, N2)
- Paging
- Configuration Update

### Q23. What is the AMF config structure in Open5GS?
**Answer:**
```yaml
amf:
  sbi:
    server:
      - address: 127.0.0.5
  ngap:
    server:
      - address: 127.0.0.5
  metrics:
    server:
      - address: 127.0.0.5
        port: 9090
  guami:
    - plmn_id:
        mcc: 001
        mnc: 01
      amf_id:
        region: 2
        set: 1
  tai:
    - plmn_id:
        mcc: 001
        mnc: 01
      tac: 1
  plmn_support:
    - plmn_id:
        mcc: 001
        mnc: 01
      s_nssai:
        - sst: 1
  security:
    integrity_order: [NIA2, NIA1, NIA0]
    ciphering_order: [NEA0, NEA1, NEA2]
  network_name:
    full: Open5GS
  amf_name: open5gs-amf0
```

### Q24. What is GUAMI?
**Answer:** GUAMI (Globally Unique AMF Identifier) = PLMN ID + AMF Region ID (8 bits) + AMF Set ID (10 bits) + AMF Pointer (6 bits). Identifies a specific AMF instance.

### Q25. What is AMF Set?
**Answer:** Group of AMFs that can serve same UE (for load sharing, redundancy). UE can move between AMFs in same set.

### Q26. What is the integrity and ciphering algorithm list?
**Answer:**
- **Integrity:** NIA0 (null), NIA1 (SNOW), NIA2 (AES), NIA3 (ZUC).
- **Ciphering:** NEA0 (null), NEA1 (SNOW), NEA2 (AES), NEA3 (ZUC).
- **NIA0/NEA0:** Should not be used in production (no security).

### Q27. What is the NGAP procedure flow for registration?
**Answer:**
1. UE -> gNB: RRC Setup Complete (with NAS Registration Request).
2. gNB -> AMF: NGAP Initial UE Message (with NAS).
3. AMF authenticates UE.
4. AMF -> gNB: NGAP DownlinkNasTransport (Auth Request).
5. ... (multiple round trips).
6. AMF -> gNB: NGAP Initial Context Setup Request (with NAS Registration Accept).
7. UE -> AMF (via gNB): Registration Complete.

### Q28. What is paging in 5G?
**Answer:** When data arrives for IDLE UE, AMF sends paging:
- AMF -> gNB: NGAP Paging.
- gNB sends paging on PDCCH.
- UE wakes up, sends Service Request.

### Q29. What is "Service Request"?
**Answer:** UE in IDLE wants to send/receive data. Sends Service Request NAS to AMF. AMF triggers UE Context setup and PDU session activation.

### Q30. What is N2 handover?
**Answer:** Used when source and target gNB don't have Xn. AMF coordinates:
1. Source gNB -> AMF: Handover Required.
2. AMF -> Target gNB: Handover Request.
3. Target gNB -> AMF: Handover Request Ack.
4. AMF -> SMF: Update PDU session (UPF path switch).
5. AMF -> Source gNB: Handover Command.
6. UE moves to Target gNB.

---

## SECTION C: SMF Deep Dive (31-40)

### Q31. What does SMF do?
**Answer:** SMF (Session Management Function):
- PDU session establishment, modification, release.
- UE IP address allocation (from pool or DHCP).
- UPF selection and control (via PFCP).
- QoS rule enforcement.
- Charging.
- DNS/DHCP for UE.

### Q32. What protocols does SMF use?
**Answer:**
- **HTTP/2** with AMF (N11), PCF (N7), UDM (N10), NRF, CHF.
- **PFCP** with UPF (N4).
- **GTP-C** can be used towards EPC (for interworking).

### Q33. What is the SMF config in Open5GS?
**Answer:**
```yaml
smf:
  sbi:
    server:
      - address: 127.0.0.4
  pfcp:
    server:
      - address: 127.0.0.4
    client:
      upf:
        - address: 127.0.0.7
  gtpc:
    server:
      - address: 127.0.0.4
  gtpu:
    server:
      - address: 127.0.0.4
  session:
    - subnet: 10.45.0.1/16
      dnn: internet
  dns:
    - 8.8.8.8
  metrics:
    server:
      - address: 127.0.0.4
        port: 9090
  info:
    - s_nssai:
        - sst: 1
          dnn:
            - internet
```

### Q34. What is PFCP and its messages?
**Answer:** PFCP (Packet Forwarding Control Protocol) on N4. UDP port 8805. Messages:
- **Heartbeat Request/Response**
- **Association Setup Request/Response** (init between SMF and UPF)
- **Session Establishment Request/Response** (create UE session)
- **Session Modification Request/Response**
- **Session Deletion Request/Response**
- **Session Report Request/Response** (UPF reports events)

### Q35. What are PDR, FAR, QER, URR in PFCP?
**Answer:** These are rules SMF sends to UPF:
- **PDR (Packet Detection Rule):** How to identify a packet (e.g., from this TEID, this 5-tuple).
- **FAR (Forwarding Action Rule):** What to do (forward, drop, buffer).
- **QER (QoS Enforcement Rule):** Rate limiting, QoS.
- **URR (Usage Reporting Rule):** Volume/time reporting (charging).
- **BAR (Buffering Action Rule):** Buffering rules.

### Q36. Example PDU session flow:
**Answer:**
1. UE -> AMF: PDU Session Establishment Request (NAS).
2. AMF -> SMF: CreateSMContext (HTTP/2 N11).
3. SMF selects UPF.
4. SMF -> UPF: PFCP Session Establishment Request (with PDR, FAR).
5. UPF allocates DL F-TEID, replies to SMF.
6. SMF -> AMF: N1N2MessageTransfer (with N2 PDU Session Resource Setup).
7. AMF -> gNB: NGAP PDU Session Resource Setup Request.
8. gNB allocates UL F-TEID, sends to AMF.
9. AMF -> SMF: UpdateSMContext.
10. SMF -> UPF: PFCP Session Modification (with gNB TEID).
11. Data flows.

### Q37. What is UE IP allocation in SMF?
**Answer:** SMF picks IP from configured pool (subnet in YAML) per DNN/slice. Alternatives:
- DHCP from external DHCP server.
- Static IP from UDM (subscription).

### Q38. What is local breakout?
**Answer:** Instead of routing UE traffic to centralized UPF, place UPF at edge (closer to user). Lower latency. Useful for MEC (Multi-access Edge Computing).

### Q39. What is UPF selection in SMF?
**Answer:** SMF selects UPF based on:
- DNN (data network name)
- S-NSSAI (slice)
- TAI (location of UE)
- UPF capabilities, load
- Configuration

### Q40. What is Ethernet PDU session?
**Answer:** New in 5G. UE sends Ethernet frames (Layer 2) over PDU session instead of IP. Use cases:
- TSN (Time-Sensitive Networking) for Industry 4.0
- Factory automation
- L2 VPN

In Open5GS, configure DNN with type=Ethernet.

---

## SECTION D: UPF Deep Dive (41-50)

### Q41. What does UPF do?
**Answer:** UPF (User Plane Function):
- GTP-U termination on N3 (from gNB).
- Packet inspection (5-tuple match using PDR).
- QoS enforcement (using QER).
- Packet forwarding to data network (N6).
- Buffering for IDLE UE.
- Charging reporting (URR).
- IP allocation (sometimes, but usually SMF).

### Q42. How does Open5GS UPF work internally?
**Answer:**
1. UPF runs as user-space process.
2. Receives GTP-U over UDP socket.
3. Decapsulates GTP-U.
4. Looks up PDR, applies FAR.
5. Writes packet to `ogstun` TUN device.
6. Kernel routes to N6 (internet) via iptables NAT.

Reverse: kernel receives from internet, sends to `ogstun`, UPF reads, encapsulates GTP-U, sends to gNB.

### Q43. What is the difference between Open5GS UPF and UPF in DPDK?
**Answer:**
- **Open5GS UPF:** Userspace, simple, slow (~Gbps). Uses TUN.
- **DPDK UPF:** Kernel bypass, fast (10s of Gbps). Used in commercial production.

### Q44. What is F-TEID?
**Answer:** F-TEID (Fully Qualified TEID) = TEID + IP address. Identifies one end of GTP-U tunnel.

### Q45. How many TEIDs are needed per PDU session?
**Answer:**
- Uplink: gNB has UL TEID (UPF -> gNB sends here? No, gNB receives here). Wait:
  - **DL TEID:** UPF -> gNB. Allocated by gNB.
  - **UL TEID:** gNB -> UPF. Allocated by UPF.

Plus, for F1-U:
- DU has separate F1-U TEIDs.

### Q46. What is the GTP-U header?
**Answer:** Minimum 8 bytes:
- Flags (1 byte): version, PT, E, S, PN.
- Message Type (1 byte): T-PDU = 0xFF.
- Length (2 bytes): payload length.
- TEID (4 bytes): tunnel ID.

Optional extensions: sequence number, N-PDU number, next extension header.

### Q47. What is GTP-U Extension Header for QoS?
**Answer:** **PDU Session Container** extension header. Contains:
- PDU Type (DL or UL)
- QFI (QoS Flow Identifier)

Used to map QoS Flow to DRB at gNB.

### Q48. What is QFI?
**Answer:** QFI (QoS Flow Identifier) is a 6-bit value (0-63). Each QoS Flow has a QFI. SDAP at gNB uses QFI to map flow to DRB.

### Q49. What is the difference between QFI and 5QI?
**Answer:**
- **5QI:** Standardized QoS profile (priority, latency, loss).
- **QFI:** Identifier of a QoS Flow in PDU session.
Multiple QFIs can map to same 5QI.

### Q50. How does UPF handle uplink classification?
**Answer:**
1. UPF receives GTP-U from gNB with QFI.
2. Looks up PDR (matches UE IP, optional 5-tuple).
3. Applies FAR (forward to N6).
4. Applies QER (rate limit based on QFI).

For DL: UPF receives IP packet from N6, matches PDR (UE IP), applies SDF filters per QoS, classifies into QFI, encapsulates with QFI, sends to gNB.

---

## SECTION E: PCF, UDM, AUSF, NSSF (51-70)

### Q51. What does PCF do?
**Answer:** PCF (Policy Control Function):
- Decides QoS policies (5QI, MBR, GBR).
- Charging policy.
- Slice-specific policies.
- Sends policy to SMF for enforcement.
- Sends AM policy to AMF (e.g., RAT restrictions).

### Q52. What is the PCF config in Open5GS?
**Answer:**
```yaml
pcf:
  sbi:
    server:
      - address: 127.0.0.13
  metrics:
    server:
      - address: 127.0.0.13
        port: 9090
  policy:
    - plmn_id:
        mcc: 001
        mnc: 01
      slice:
        - sst: 1
          default_indicator: true
          session:
            - name: internet
              type: 3
              ambr:
                downlink:
                  value: 1
                  unit: 3   # Gbps
                uplink:
                  value: 1
                  unit: 3
              qos:
                index: 9    # 5QI 9
                arp:
                  priority_level: 8
                  pre_emption_vulnerability: 1
                  pre_emption_capability: 1
```

### Q53. What does UDM do?
**Answer:** UDM (Unified Data Management):
- Stores subscription data (or retrieves from UDR).
- Generates auth vectors.
- Provides UE info to other NFs.
- Identifier de-concealment (SUCI to SUPI).

### Q54. What does UDR do?
**Answer:** UDR (Unified Data Repository) is the database backend for UDM, PCF, NEF. Open5GS uses MongoDB.

### Q55. What does AUSF do?
**Answer:** AUSF (Authentication Server Function):
- Authenticates UE using 5G-AKA or EAP-AKA'.
- Gets auth vectors from UDM.
- Generates KSEAF (security anchor key).

### Q56. What is NSSF?
**Answer:** NSSF (Network Slice Selection Function):
- Selects appropriate slice for UE.
- Returns allowed NSSAI, target AMF.
- Handles slice mapping for roaming.

### Q57. What is NEF?
**Answer:** NEF (Network Exposure Function):
- Exposes 3GPP capabilities as APIs to 3rd party apps.
- Examples: device location, QoS-on-demand for OTT apps.

### Q58. What is NWDAF?
**Answer:** NWDAF (Network Data Analytics Function):
- Collects data from NFs.
- Provides analytics (load, mobility, QoE).
- Optional NF, useful for AI/ML.

### Q59. What is CHF?
**Answer:** CHF (Charging Function) - online/offline charging. Replaces OCS/OFCS in 4G.

### Q60. What is SMSF?
**Answer:** SMSF (Short Message Service Function) - handles SMS over NAS.

### Q61. What is the 5G authentication flow (5G-AKA)?
**Answer:**
1. UE -> AMF: Registration Request (with SUCI).
2. AMF -> AUSF: UE Authentication Request (SUCI).
3. AUSF -> UDM: Get auth vector (SUCI).
4. UDM resolves SUCI to SUPI, generates auth vector (RAND, AUTN, XRES*, K_AUSF), returns.
5. AUSF -> AMF: Auth Request (RAND, AUTN).
6. AMF -> UE: NAS Auth Request.
7. UE checks AUTN, computes RES*, sends to AMF.
8. AMF forwards RES* to AUSF.
9. AUSF compares with XRES*. If match, sends K_SEAF to AMF.
10. AMF derives K_AMF, K_NAS, K_gNB.

### Q62. What keys are derived in 5G?
**Answer:** Key hierarchy:
- **K (SIM):** Permanent secret.
- **CK, IK:** From K (auth).
- **K_AUSF:** Auth Server key.
- **K_SEAF:** Anchor key.
- **K_AMF:** AMF key.
- **K_NASint, K_NASenc:** NAS keys.
- **K_gNB:** Base station key.
- **K_RRCint, K_RRCenc, K_UPint, K_UPenc:** RRC/UP keys.

### Q63. How to add a subscriber via mongo CLI?
**Answer:**
```bash
mongosh open5gs
db.subscribers.insertOne({
  imsi: "001010000000001",
  msisdn: ["0000000001"],
  security: { k: "...", opc: "...", amf: "8000" },
  ambr: { downlink: { value: 1, unit: 3 }, uplink: { value: 1, unit: 3 } },
  slice: [{
    sst: 1,
    default_indicator: true,
    session: [{
      name: "internet",
      type: 3,
      qos: { index: 9, arp: { priority_level: 8, pre_emption_vulnerability: 1, pre_emption_capability: 1 } },
      ambr: { downlink: { value: 1, unit: 3 }, uplink: { value: 1, unit: 3 } }
    }]
  }]
})
```

### Q64. How does Open5GS handle network slicing?
**Answer:**
- Multiple S-NSSAI configured per NF.
- Subscriber data has allowed slices.
- AMF picks SMF per slice (or NSSF helps).
- SMF picks UPF per slice (UPF can be slice-dedicated).
- Each slice has own DNN, QoS, IP pool.

### Q65. How to test multiple slices?
**Answer:**
1. Configure 2+ slices in AMF, SMF, NSSF.
2. Provision subscriber with multiple S-NSSAI in DB.
3. Configure UERANSIM UE with `requestedNssai`.
4. UE registers and gets `allowedNssai`.
5. PDU sessions established per slice.

### Q66. What is the difference between requested and allowed NSSAI?
**Answer:**
- **Requested NSSAI:** UE sends in Registration Request.
- **Allowed NSSAI:** AMF (via NSSF) returns based on subscription, PLMN, current location.
- **Default NSSAI:** Used if UE doesn't request any.
- **Configured NSSAI:** Stored by UE for future PLMNs.

### Q67. What is the role of NRF in service discovery?
**Answer:** Every NF registers with NRF when it starts. Other NFs query NRF:
```http
GET /nnrf-disc/v1/nf-instances?target-nf-type=SMF&requester-nf-type=AMF&service-names=nsmf-pdusession
```
NRF returns list of matching NFs.

### Q68. What information does NF send in NFRegister?
**Answer:**
- NF Instance ID (UUID)
- NF Type (AMF, SMF, etc.)
- PLMN list
- S-NSSAI list
- FQDN, IPv4/v6 addresses
- Services (with URI and supported versions)
- Capacity, load, status

### Q69. What is "indirect communication" via SCP?
**Answer:** Instead of AMF -> SMF direct, AMF -> SCP -> SMF. SCP:
- Discovers SMF via NRF.
- Routes message.
- Provides resilience.

Useful in large deployments.

### Q70. What is the typical Open5GS deployment topology?
**Answer:** Two common setups:
**Single host (test):** All NFs on one Linux box, using 127.0.0.X loopback addresses.
**Distributed:** Each NF as Docker container or VM. NFs in same network namespace or via separate IPs. Plus MongoDB cluster.

---

## END OF LEVEL 4 (70 questions)

> Move to **05_PROTOCOLS_DEEP.md** for protocol details.
