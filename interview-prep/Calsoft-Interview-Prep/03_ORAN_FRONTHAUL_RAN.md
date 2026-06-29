# Level 3: O-RAN, Fronthaul, srsRAN, OCUDU (Senior RAN Developer)

> Direct match to JD: "OCUDU/srsRAN deployment & config, O-RAN 7.2 open fronthaul (OFH), CU/DU split (F1/E1 interfaces), 3GPP Rel-17 RAN features"

---

## SECTION A: O-RAN Basics (1-20)

### Q1. What is O-RAN?
**Answer:** O-RAN (Open RAN) is an open standard for building RAN with interoperable components from different vendors. Created by O-RAN Alliance. Goal: break vendor lock-in.

### Q2. What is the difference between 3GPP RAN and O-RAN?
**Answer:**
- **3GPP RAN:** Defines RAN architecture, but interfaces between RU/DU/CU can be proprietary.
- **O-RAN:** Adds OPEN interfaces (especially Open Fronthaul) so different vendors can interoperate. Adds RIC (RAN Intelligent Controller) for AI/ML.

### Q3. What is the O-RAN architecture?
**Answer:**
```
         [Non-RT RIC] --A1-->
              |
         [Near-RT RIC] --E2--> [O-CU, O-DU, O-RU]
                                  |
                              [O-Cloud]
```
- **O-RU:** Open Radio Unit
- **O-DU:** Open Distributed Unit
- **O-CU:** Open Central Unit (split into O-CU-CP and O-CU-UP)
- **Near-RT RIC:** Near Real-Time RAN Intelligent Controller
- **Non-RT RIC:** Non Real-Time RIC (in SMO)
- **SMO:** Service Management and Orchestration

### Q4. What are O-RAN Working Groups?
**Answer:** Important ones:
- **WG1:** Use Cases & Overall Architecture
- **WG2:** Non-RT RIC and A1 interface
- **WG3:** Near-RT RIC and E2 interface
- **WG4:** Open Fronthaul interface (most important for RAN dev)
- **WG5:** Open F1/W1/E1/X2/Xn
- **WG6:** Cloudification (O-Cloud)
- **WG7:** White-box hardware
- **WG8:** Stack reference design
- **WG9:** Transport
- **WG10:** OAM
- **WG11:** Security

### Q5. What is Open Fronthaul (OFH)?
**Answer:** Open Fronthaul is the open interface between O-DU and O-RU. Specified by O-RAN WG4. Uses Option 7.2x split.
4 planes:
- **C-plane:** Control (scheduling, beamforming)
- **U-plane:** User data (IQ samples)
- **S-plane:** Synchronization (PTP, SyncE)
- **M-plane:** Management (NETCONF/YANG)

### Q6. What is the 7.2x split exactly?
**Answer:** PHY layer is split:
- **Higher PHY** in O-DU: PDSCH/PUSCH coding, scrambling, modulation, layer mapping.
- **Lower PHY** in O-RU: Precoding (optional), Resource Element mapping, IFFT/FFT, CP, beamforming.

### Q7. What are Category A and Category B O-RU?
**Answer:**
- **Cat A O-RU:** No precoding in RU. DU sends per-layer data, RU just does mapping + IFFT. Simpler RU.
- **Cat B O-RU:** Precoding in RU. DU sends per-port data. RU has precoding matrix. More complex but lower bandwidth.

### Q8. What protocols are used on Open Fronthaul?
**Answer:**
- **U-plane and C-plane:** eCPRI over Ethernet (Layer 2). Roe (Radio over Ethernet) optional.
- **S-plane:** PTP (IEEE 1588) + SyncE.
- **M-plane:** NETCONF/YANG over SSH/TLS over IP.

### Q9. What is eCPRI?
**Answer:** eCPRI is a low-bandwidth protocol for fronthaul. Carries:
- IQ data (User plane)
- Real-time control (C-plane)
Runs over Ethernet. Uses message types like 0x00 (IQ data), 0x02 (real-time control), 0x05 (one-way delay measurement).

### Q10. What is the eCPRI header?
**Answer:** 4-byte header:
- Version (4 bits)
- Reserved (3 bits)
- Concatenation (1 bit)
- Message Type (1 byte)
- Payload Size (2 bytes)

Then comes payload.

### Q11. What is the bandwidth of fronthaul for 100 MHz cell?
**Answer:** Roughly:
- **Cat A, 4T4R, 100 MHz:** ~15-20 Gbps DL.
- **Cat B:** lower (~5-10 Gbps) because of precoding in RU.
Depends on IQ compression too.

### Q12. What is IQ compression in O-RAN?
**Answer:** Reduces fronthaul bandwidth by compressing IQ samples:
- **Block Floating Point (BFP):** Common, simple.
- **Modulation Compression:** For PDSCH only.
- **u-law / Block Scaling**

### Q13. What is the M-plane?
**Answer:** M-plane manages O-RU configuration. Uses NETCONF protocol with YANG data models. SSH/TLS for security.

Two architectures:
- **Hierarchical:** DU manages RU directly.
- **Hybrid:** SMO/NMS manages RU directly, DU only handles real-time.

### Q14. What is RIC?
**Answer:** RIC (RAN Intelligent Controller) brings AI/ML to RAN.
- **Non-RT RIC:** >1 second loops. Inside SMO. Hosts rApps.
- **Near-RT RIC:** 10ms - 1s loops. Hosts xApps. Controls RAN via E2.

### Q15. What is xApp and rApp?
**Answer:**
- **xApp:** Microservice on Near-RT RIC. Uses E2 to control RAN (e.g., traffic steering, anomaly detection).
- **rApp:** App on Non-RT RIC. Uses A1 to send policies to Near-RT RIC.

### Q16. What is A1 interface?
**Answer:** A1 is between Non-RT RIC (SMO) and Near-RT RIC. Used to send policies, ML models, enrichment info. Uses HTTP/REST.

### Q17. What is E2 interface?
**Answer:** E2 is between Near-RT RIC and RAN nodes (O-CU, O-DU). Uses E2AP over SCTP. Two services:
- **E2SM-KPM:** KPI monitoring.
- **E2SM-RC:** RAN control.

### Q18. What is O1 interface?
**Answer:** O1 is between SMO and O-RAN nodes for management. Uses NETCONF/YANG. Replaces traditional FCAPS interface.

### Q19. What is O2 interface?
**Answer:** O2 is between SMO and O-Cloud. Manages cloud infrastructure (VMs, containers).

### Q20. What is O-Cloud?
**Answer:** O-Cloud is the cloud infrastructure where O-RAN VNFs/CNFs (CU, DU) run. Can be K8s, OpenStack, or bare metal with hypervisor.

---

## SECTION B: srsRAN (21-40)

### Q21. What is srsRAN?
**Answer:** srsRAN is an open-source 4G/5G software RAN by Software Radio Systems. Two main projects:
- **srsRAN 4G** (older, full LTE + early NSA)
- **srsRAN Project / srsRAN gNB** (5G SA, modern, O-RAN compliant)

### Q22. What is the difference between srsRAN 4G and srsRAN Project?
**Answer:**
- **srsRAN 4G:** Older codebase. Has eNB, EPC, UE. Mainly LTE.
- **srsRAN Project:** New codebase. Only gNB (5G SA). Connects to external 5GC like Open5GS. Better O-RAN support.

### Q23. How do you deploy srsRAN gNB?
**Answer:** Steps:
1. Install dependencies (cmake, gcc, libfftw3, libsctp, etc.)
2. Clone srsRAN Project from GitHub.
3. Build with cmake + make.
4. Configure `gnb.yml` (cell config, AMF address, RU config).
5. Run `sudo ./gnb -c gnb.yml`.

### Q24. What hardware is supported by srsRAN?
**Answer:**
- USRP (B210, X310, N310, N320) - via UHD driver.
- BladeRF.
- ZMQ (for software simulation, no radio).
- Now: ORAN 7.2 RU via DPDK + DPDK-based fronthaul.

### Q25. What is ZeroMQ (ZMQ) in srsRAN?
**Answer:** ZMQ is a messaging library. In srsRAN, ZMQ is used to send IQ samples between gNB and UE (or RU emulator) over IP - no real radio needed. Great for testing in Docker.

### Q26. How do you configure ZMQ in srsRAN?
**Answer:** In gnb.yml:
```yaml
ru_sdr:
  device_driver: zmq
  device_args: tx_port=tcp://*:2000,rx_port=tcp://localhost:2001,base_srate=11.52e6
```
UE side uses opposite ports.

### Q27. What is the basic gnb.yml structure?
**Answer:**
```yaml
cu_cp:
  amf:
    addr: 127.0.0.5
    bind_addr: 127.0.0.4
ru_sdr:
  device_driver: uhd
cell_cfg:
  dl_arfcn: 368500
  band: 3
  channel_bandwidth_MHz: 20
  common_scs: 15
  plmn: "00101"
  tac: 7
```

### Q28. How do you connect srsRAN to Open5GS?
**Answer:**
1. Install and run Open5GS (AMF, SMF, UPF, etc.).
2. In gnb.yml, set AMF IP address (`cu_cp.amf.addr`).
3. Make sure PLMN matches in both.
4. Provision UE in Open5GS WebUI (IMSI, key, OPC).
5. Start gNB - it will set up N2 (NGAP/SCTP) with AMF.
6. Start UE (srsUE or COTS) - it will register.

### Q29. What is srsUE?
**Answer:** srsUE is a software UE in srsRAN. Simulates a 4G/5G phone. Useful for testing without real phone.

### Q30. How do you run srsUE?
**Answer:**
```bash
sudo ./srsue ue.conf
```
Config has IMSI, key, USIM type, ZMQ/USRP, frequency.

### Q31. What is OCUDU?
**Answer:** OCUDU is likely shorthand for O-CU + O-DU - the combined or single binary for O-RAN-compliant gNB. srsRAN gNB is essentially O-CU+O-DU. Could also be a Calsoft product name. Always confirm with interviewer.

### Q32. How does srsRAN handle CU-DU split?
**Answer:** srsRAN Project gNB is monolithic by default (CU+DU in one binary). For split, F1AP is supported - separate CU and DU binaries can be started with F1 between them.

### Q33. How do you debug srsRAN issues?
**Answer:**
- Check logs: `/tmp/gnb.log` (set log level to debug in yml).
- Use Wireshark on loopback or SCTP capture.
- Use `tcpdump -i lo -w gnb.pcap`.
- Check kernel logs: `dmesg`.
- For radio issues: USRP sample rate, master clock, gain.

### Q34. What CPU pinning is needed for srsRAN?
**Answer:** Real-time threads need dedicated cores. In gnb.yml:
```yaml
expert_execution:
  cpu_isolation: 1,2,3
  affinities:
    low_priority_threads:
      mask: [0]
    ru_timing_cpu: 1
    ofh_cpus: 2,3
```

### Q35. Why CPU isolation important in srsRAN?
**Answer:** RAN has tight timing (1ms slots). If kernel scheduler moves threads, samples are dropped. CPU isolation (`isolcpus` kernel boot) keeps cores reserved for srsRAN.

### Q36. What is master clock rate in srsRAN?
**Answer:** USRP master clock is set in config. For B210, typically 23.04 MHz or 11.52 MHz. Must match the cell bandwidth (multiple of sample rate).

### Q37. How does srsRAN support O-RAN fronthaul?
**Answer:** srsRAN Project supports O-RAN 7.2 split via DPDK. Configure in ru_ofh section:
```yaml
ru_ofh:
  ru_bandwidth_MHz: 100
  t1a_max_cp_dl: 470
  ta4_max: 360
  is_prach_cp_enabled: true
  compr_method_ul: bfp
  compr_bitwidth_ul: 9
  cells:
    - network_interface: 0000:b1:00.1
      ru_mac_addr: aa:bb:cc:dd:ee:ff
      du_mac_addr: 11:22:33:44:55:66
      vlan_tag_cp: 5
      vlan_tag_up: 5
```

### Q38. What is DPDK and why used in O-RAN?
**Answer:** DPDK (Data Plane Development Kit) bypasses Linux kernel for packet processing. Apps directly access NIC. Critical for O-RAN fronthaul because of strict timing (sub-millisecond).

### Q39. What is PMD in DPDK?
**Answer:** PMD (Poll Mode Driver) is a userspace driver in DPDK. CPU polls NIC instead of using interrupts. Reduces latency.

### Q40. What kernel parameters are needed for srsRAN with DPDK?
**Answer:**
- `isolcpus=1,2,3,4` - isolate CPUs
- `intel_iommu=on` - for VFIO
- `iommu=pt` - passthrough
- `hugepages=4096` - 1G hugepages for DPDK
- `nohz_full=1,2,3,4` - no tick on isolated CPUs
- `rcu_nocbs=1,2,3,4` - move RCU callbacks

---

## SECTION C: F1 and E1 Interfaces (41-55)

### Q41. What does F1 carry?
**Answer:**
- **F1-C (control plane):** F1AP messages over SCTP. UE context, RRC transport, bearer setup.
- **F1-U (user plane):** GTP-U tunnels carrying PDCP PDUs from CU to DU.

### Q42. Name some F1AP messages.
**Answer:**
- **F1 Setup Request/Response:** Initial connection between CU and DU.
- **UE Context Setup Request/Response:** Create UE context in DU.
- **UE Context Modification:** Change bearer/DRB.
- **Initial UL RRC Message Transfer:** Forward UE's RRC to CU.
- **DL/UL RRC Message Transfer:** RRC messages between CU and DU.

### Q43. What is the SCTP port for F1?
**Answer:** F1AP SCTP port: **38472**.

### Q44. What does E1 carry?
**Answer:** E1AP messages between CU-CP and CU-UP. Topics:
- Bearer setup (which DRB on CU-UP)
- Security info (PDCP keys)
- Statistics

### Q45. Name some E1AP messages.
**Answer:**
- **E1 Setup Request/Response**
- **Bearer Context Setup Request/Response**
- **Bearer Context Modification**
- **Bearer Context Release**

### Q46. What is the SCTP port for E1?
**Answer:** E1AP SCTP port: **38462**.

### Q47. What is the F1AP UE association?
**Answer:** Each UE has a unique:
- **gNB-CU UE F1AP ID** (CU side)
- **gNB-DU UE F1AP ID** (DU side)
Used to identify UE in F1AP messages.

### Q48. How is RRC sent over F1?
**Answer:** RRC is encoded in DU/CU, then put into F1AP message:
- **Initial UL RRC Message Transfer:** First RRC from UE.
- **DL RRC Message Transfer / UL RRC Message Transfer:** Subsequent RRC.

DU only relays, CU processes RRC.

### Q49. What is the role of CU and DU in handover?
**Answer:**
- **Intra-DU handover:** Internal to DU, no F1AP needed.
- **Intra-CU inter-DU handover:** CU triggers, sends UE Context Setup to new DU.
- **Inter-CU handover:** Goes via Xn between CUs.

### Q50. What is the difference between F1-C and Xn-C?
**Answer:**
- **F1-C:** Within same gNB (CU to DU).
- **Xn-C:** Between two gNBs (CU to CU).

### Q51. How is QoS handled across F1?
**Answer:** CU configures QoS flows in DRB via F1AP UE Context Setup. Each DRB has a QoS profile. DU enforces QoS in MAC scheduler.

### Q52. What is the F1-U tunnel?
**Answer:** GTP-U tunnel between CU-UP and DU for user data. Each DRB has its own F1-U TEID.

### Q53. How are TEIDs assigned in F1?
**Answer:**
- DU allocates DL TEID, gives to CU in UE Context Setup Response.
- CU allocates UL TEID, gives to DU in UE Context Setup Request.

### Q54. What if F1-C SCTP connection fails?
**Answer:** All UEs in DU are dropped. DU re-attempts F1 Setup with CU. Critical for service continuity - SCTP multi-homing helps.

### Q55. Does srsRAN support F1 split?
**Answer:** Yes, srsRAN Project supports F1 split. You can run `gnb` with `--ru.amf` for CU only and separate DU binary, with F1 between.

---

## SECTION D: RRC and Procedures (56-70)

### Q56. What are RRC states in 5G NR?
**Answer:**
- **RRC_IDLE:** UE not connected.
- **RRC_INACTIVE:** UE inactive but RAN context preserved. Faster resume.
- **RRC_CONNECTED:** Active connection.

### Q57. What is RRC_INACTIVE benefit?
**Answer:**
- Faster transition to CONNECTED (no full setup).
- Saves battery in UE.
- Reduces signaling load.

### Q58. What RRC messages are in initial attach?
**Answer:**
1. **RRC Setup Request** (UE -> gNB on SRB0)
2. **RRC Setup** (gNB -> UE)
3. **RRC Setup Complete** (UE -> gNB) - contains NAS Registration Request.
4. **DL Information Transfer / UL Information Transfer** - NAS messages.
5. **Security Mode Command/Complete**
6. **RRC Reconfiguration** (with DRB setup)
7. **RRC Reconfiguration Complete**

### Q59. What is RRC Reconfiguration?
**Answer:** Most important RRC message. Used to:
- Setup/modify/release SRBs and DRBs
- Configure measurement
- Perform handover
- Reconfigure security

### Q60. What is RRC Setup vs RRC Reestablishment vs RRC Resume?
**Answer:**
- **RRC Setup:** Initial connection from IDLE.
- **RRC Reestablishment:** Connection recovery after failure (e.g., RLF).
- **RRC Resume:** Resume from INACTIVE state.

### Q61. What is MIB and SIB?
**Answer:**
- **MIB (Master Information Block):** On PBCH. Minimum info (frame number, SCS, k_SSB).
- **SIB (System Information Block):** On PDSCH. Cell config (SIB1, SIB2-9).

### Q62. What is SIB1?
**Answer:** SIB1 (also called RMSI) is scheduled by SIB1 PDCCH. Contains:
- Cell selection info
- PLMN list
- Cell barring info
- Random access config
- Scheduling for other SIBs

### Q63. What is random access procedure?
**Answer:** When UE wants to connect:
1. UE sends **Msg1 (PRACH preamble)**.
2. gNB replies with **Msg2 (RAR - Random Access Response)** on PDSCH.
3. UE sends **Msg3 (RRC Setup Request)** on PUSCH.
4. gNB sends **Msg4 (RRC Setup / Contention Resolution)**.

### Q64. What is 2-step RACH?
**Answer:** New in 5G. Combines steps:
- **MsgA:** Preamble + payload.
- **MsgB:** Response with contention resolution.
Faster than 4-step. Used for small data, fast access.

### Q65. What is CFRA and CBRA?
**Answer:**
- **CBRA (Contention-Based RA):** Multiple UEs may pick same preamble. Conflict resolution needed.
- **CFRA (Contention-Free RA):** Network assigns dedicated preamble (used in handover, no contention).

### Q66. What is BWP?
**Answer:** BWP (Bandwidth Part) is a portion of cell bandwidth. UE can be configured with up to 4 BWPs but only 1 active at a time. Helps low-cost UEs not handle full BW.

### Q67. What is CORESET?
**Answer:** CORESET (Control Resource Set) is the time-frequency region where PDCCH (control) is searched. Configured by SIB1 or RRC.

### Q68. What is search space?
**Answer:** Search space tells UE which PDCCH candidates to monitor in CORESET. Two types:
- **Common search space (CSS):** Common DCI (SIB, paging).
- **UE-specific search space (USS):** UE-specific DCI.

### Q69. What is DCI?
**Answer:** DCI (Downlink Control Information) carried in PDCCH. Tells UE about scheduling (DCI 0_0, 0_1 for UL grant; 1_0, 1_1 for DL assignment).

### Q70. What is UCI?
**Answer:** UCI (Uplink Control Information) sent on PUCCH. Contains:
- HARQ ACK/NACK
- CSI (Channel State Information)
- SR (Scheduling Request)

---

## END OF LEVEL 3 (70 questions)

> Move to **04_OPEN5GS_CORE_NFs.md** for Core role.
