# Level 10: Scenario-Based & Troubleshooting Questions

> These are real questions interviewers love. Shows you understand the WHY, not just memorized definitions.

---

## SECTION A: Setup & Deployment Scenarios (1-15)

### Q1. You have srsRAN gNB and Open5GS in same Ubuntu box. UE doesn't register. What do you check first?
**Answer:** Check in this order:
1. **Open5GS NFs running?** `systemctl status open5gs-*` or `ps aux | grep open5gs`.
2. **AMF reachable?** Check `gnb.yml` has correct AMF IP. `nc -z 127.0.0.5 38412` may not work for SCTP — use `tcpdump -i any sctp`.
3. **SCTP association up?** `cat /proc/net/sctp/eps` and `assocs`. Or look at NG Setup in Wireshark.
4. **PLMN match?** Both srsRAN `gnb.yml` and Open5GS `amf.yaml` must have same MCC/MNC.
5. **TAC match?** Configured TAC must be in AMF's supported list.
6. **Subscriber provisioned?** Check WebUI or MongoDB.
7. **IMSI/key/OPC match?** UE config and DB must match exactly.
8. **NSSAI match?** Subscriber's slice and AMF's slice must intersect.
9. **Logs:** `/var/log/open5gs/amf.log` and srsRAN gNB log.

### Q2. UE registered but PDU session fails. What now?
**Answer:**
1. **SMF reachable from AMF?** Check NRF registration. SMF must be registered. `curl http://127.0.0.10:7777/nnrf-nfm/v1/nf-instances` (NRF API).
2. **DNN match?** UE requested DNN must be in subscriber's allowed DNN list AND SMF's configured DNN.
3. **Subnet & TUN:** `ogstun` up? `ip addr show ogstun`. IP pool not exhausted.
4. **UPF reachable from SMF?** PFCP association up? `ss -ulnp | grep 8805`.
5. **N3 reachable?** gNB IP must reach UPF IP. Check routing.
6. **IP forwarding & NAT:** `sysctl net.ipv4.ip_forward` should be 1. iptables MASQUERADE present.
7. Check `/var/log/open5gs/smf.log` for rejection reason.

### Q3. UE has IP but no internet. What's wrong?
**Answer:**
1. **NAT issue:** `iptables -t nat -L POSTROUTING -v -n` — see if MASQUERADE rule exists and matches packets.
2. **IP forwarding:** must be enabled.
3. **DNS:** Open5GS gives DNS in SMF config. Check it works from UE.
4. **Firewall:** `iptables -L` — make sure FORWARD chain allows traffic.
5. **Default gateway in ogstun subnet:** UE should have default route via UPF IP.
6. From host, try ping internet via ogstun IP source: `ping -I 10.45.0.1 8.8.8.8`.
7. Capture on ogstun: `tcpdump -i ogstun` — see if packets arrive from UE.

### Q4. NGAP NG Setup keeps failing. What to check?
**Answer:**
1. SCTP packet capture — see if INIT/INIT-ACK is happening.
2. PLMN mismatch — most common reason for NG Setup Failure.
3. TAC/Supported TAs — list of (PLMN, TAC) must overlap.
4. Slice (S-NSSAI) — at least one S-NSSAI must overlap.
5. AMF reachability — gNB can reach AMF IP.
6. Firewall — SCTP often blocked.
7. Check NGAP cause code in NG Setup Failure message.

### Q5. SCTP association establishes but no NGAP messages exchanged. What now?
**Answer:**
1. PPID mismatch — gNB might be sending wrong SCTP PPID (NGAP = 60).
2. NG Setup Request actually being sent? Capture to confirm.
3. AMF maybe rejecting silently — check AMF log for errors.
4. Wireshark may not auto-decode; manually "Decode As" -> NGAP.
5. Check protocol version, message format errors.

### Q6. Open5GS upgraded but old configs don't work. What changed?
**Answer:** Open5GS YAML structure changed across versions:
- v2.4 → v2.5 → v2.6 reorganized many fields (sbi, ngap, gtpu).
- Check release notes on GitHub.
- Migrate configs using diff tool (`/etc/open5gs/sample.yaml`).

### Q7. UE attaches but data stops after some time. What to investigate?
**Answer:**
1. **MongoDB issue:** If DB connection drops, NFs may misbehave. Check `journalctl -u mongod`.
2. **Inactivity timer:** UE might go to IDLE state. Check `Inactivity Timer` in AMF (default ~600s).
3. **PDR/FAR aging:** UPF rules may have expired.
4. **PFCP heartbeat:** If SMF<->UPF heartbeat fails, session torn down. Check logs.
5. **GTP-U Echo:** Same between gNB and UPF.
6. **NAT timeout:** Long-idle TCP may be cleaned by iptables conntrack.

### Q8. fronthaul timing problem in O-RAN — what to debug?
**Answer:**
1. **GM available?** `pmc -u -b 0 'GET CURRENT_DATA_SET'` to check GM presence.
2. **ptp4l running?** `journalctl -u ptp4l`.
3. **PHC offset stable?** Logs should show offset in ns, not jumping.
4. **Network path:** Is every switch a T-BC or T-TC?
5. **PTP profile match:** All nodes G.8275.1?
6. **VLAN/QoS:** PTP often on specific VLAN with high priority.
7. **GPS lock:** If GM uses GPS, antenna lock OK?

### Q9. Open5GS UPF is dropping packets. What to check?
**Answer:**
1. **CPU usage:** `top` — UPF maxed?
2. **TUN MTU:** `ip link show ogstun` — fragmentation?
3. **Conntrack table full?** `cat /proc/sys/net/netfilter/nf_conntrack_count` vs `_max`.
4. **GTP-U malformed packets:** Wireshark might show errors.
5. **PFCP rules:** All correct PDR/FAR for active sessions?

### Q10. UE registration is very slow (10+ seconds). Why?
**Answer:**
1. **NF response delay:** Check timestamps in AMF log between messages.
2. **MongoDB slow:** Query taking long.
3. **NRF discovery:** AMF/SMF doing NRF lookups for every msg.
4. **Network latency:** If NFs distributed, RTT between them adds up.
5. **Random Access delay:** SIB1/RRC retransmissions.

### Q11. Two cells on same gNB, handover not working. Debug?
**Answer:**
1. **Intra-DU:** Should not need F1, but CU still in loop.
2. **Measurement config:** UE not measuring neighbor properly.
3. **Neighbor cell list:** Configured in SIB or via RRC?
4. **Cell quality:** UE actually sees neighbor strong enough?
5. **Handover thresholds:** A3 event offset too high.
6. RRC Reconfiguration with mobility info reaching UE?

### Q12. Open5GS doesn't start, no logs. What to do?
**Answer:**
1. Run binary manually with `-c` config: `open5gs-amfd -c /etc/open5gs/amf.yaml`.
2. Validate YAML syntax: `yamllint amf.yaml`.
3. Check port conflicts: `ss -tulnp | grep 38412`.
4. Check MongoDB up.
5. Run with strace: `strace -f open5gs-amfd 2>&1 | head -100`.

### Q13. SCTP module not loading.
**Answer:**
```bash
sudo modprobe sctp
lsmod | grep sctp
# If not loaded:
sudo apt install linux-modules-extra-$(uname -r)
sudo modprobe sctp
```
Some minimal kernels don't have SCTP enabled.

### Q14. Multiple UEs cause UPF to crash.
**Answer:**
1. **File descriptors:** `ulimit -n` — increase to 65535+.
2. **Memory:** Out-of-memory? Check `dmesg`.
3. **Connection table size:** UPF has internal session limit.
4. **DPDK pkt mempool:** Too small.
5. Run with `-f -d` to see crash log. Maybe Valgrind catches it.

### Q15. New 5QI value (e.g., 82) not working — UE doesn't use DC-GBR.
**Answer:**
1. Subscriber's slice config must include the 5QI.
2. PCF policy must reference 5QI 82.
3. SMF must support 5QI 82 (newer Open5GS versions).
4. gNB must support DC-GBR (srsRAN/COTS gNB).
5. RLC mode appropriately configured (UM for low latency).

---

## SECTION B: Conceptual / Design Scenarios (16-30)

### Q16. Design: How would you deploy UPF at edge for low latency?
**Answer:**
1. Place UPF near user (edge data center, ~10 km).
2. SMF stays central.
3. SMF selects edge UPF based on UE TAI + DNN.
4. N4 (PFCP) between central SMF and edge UPF.
5. N6 from edge UPF to local internet break-out / MEC apps.
6. Low latency for MEC apps (10-20 ms).

### Q17. Why is fronthaul split 7.2x better than CPRI option 8 for 5G?
**Answer:**
- **CPRI:** Time-domain raw samples. Massive bandwidth (100s of Gbps for one cell).
- **7.2x:** Frequency-domain after FFT. ~10x less bandwidth.
- **7.2x:** Ethernet-based, multi-vendor.
- **7.2x:** Lower RU complexity (no need to do all PHY).

### Q18. How would you achieve 1 ms latency in 5G URLLC?
**Answer:**
- Short numerology (60/120 kHz SCS).
- Mini-slots (2-4 symbols).
- HARQ with low rounds.
- 2-step RACH.
- Pre-emption (puncture eMBB).
- Edge UPF (low E2E delay).
- DC-GBR 5QI.
- RLC UM (no retransmit delay).

### Q19. How is QoS enforced end-to-end from UE to internet?
**Answer:**
- **UE:** UL packets tagged with QFI (via NAS QoS rules).
- **gNB (SDAP):** Maps QFI to DRB.
- **gNB MAC:** Schedules per QoS profile (priority, GBR).
- **gNB-UPF N3:** GTP-U with PDU Session Container (carries QFI).
- **UPF:** QER enforces MBR/GBR per QFI.
- **UPF N6:** No QoS (best effort to internet).

### Q20. Why does 5G use HTTP/2 SBA instead of Diameter (4G)?
**Answer:**
- HTTP/2 is mainstream (cloud-native), easier to find devs.
- REST APIs, easier to extend (add new versions).
- Better tooling (Swagger/OpenAPI).
- Microservice friendly.
- Diameter is complex, telecom-specific.
- HTTP/2 with mutual TLS for security.

### Q21. Explain end-to-end of a UE making a video call in 5G.
**Answer:**
1. UE registered via AMF.
2. UE establishes PDU session with DNN=ims (S-NSSAI for voice/video).
3. PCF assigns QoS: 5QI 1 (voice) + 5QI 2 (video). Each maps to a QoS Flow.
4. gNB creates dedicated DRBs (GBR, RLC UM for voice).
5. SIP/SDP exchanged with IMS (P-CSCF, S-CSCF).
6. RTP packets flow on the GBR DRBs.

### Q22. How can you increase capacity in a 5G cell?
**Answer:**
- Wider bandwidth (100 MHz → 400 MHz in mmWave).
- More antennas (massive MIMO).
- Higher modulation (256-QAM, 1024-QAM).
- Carrier Aggregation (combine multiple bands).
- More cells per area (densification).
- Better scheduling (proportional fair).
- O-RAN intelligent traffic steering via RIC.

### Q23. UE moves between two gNBs. What is the procedure?
**Answer:**
1. UE measures neighbor (event A3: neighbor better than serving by X dB).
2. UE sends Measurement Report to source gNB.
3. Source gNB decides handover.
4. **Xn handover (if Xn exists):** Source asks target directly. Target prepares resources.
5. **N2 handover (if no Xn):** AMF coordinates.
6. Source -> UE: RRC Reconfiguration (handover command).
7. UE syncs to target. Sends RRC Reconfiguration Complete.
8. N3 path switched: target gNB ↔ UPF.
9. Source gNB releases UE.

### Q24. What's the difference between cloud-native NF and traditional NF?
**Answer:**
- **Cloud-native:** Microservices, container-based (Docker), Kubernetes orchestrated, stateless or DB-backed, scaling via autoscaler.
- **Traditional:** Monolithic, VM-based or bare metal, manual scaling.

5G Core SBA encourages cloud-native.

### Q25. How does srsRAN simulate UE without real hardware?
**Answer:**
- Uses ZeroMQ for IQ sample transport over IP.
- gNB and UE bind to ZMQ ports.
- Samples are exchanged as ZMQ messages.
- No air interface, but PHY/MAC/RLC fully exercised.

### Q26. How to verify QoS is actually being enforced for 5QI 82?
**Answer:**
1. Configure 5QI 82 GBR=1 Mbps.
2. Send traffic at 5 Mbps from UE.
3. Capture on N3 — see if rate is limited.
4. Tag SDF filter with marker; iperf/UDP test on UE.
5. Look at UPF QER stats (some implementations expose).
6. Verify latency under load.

### Q27. UPF placement: when to use central vs distributed?
**Answer:**
- **Central UPF:** Simple, easy to manage. OK for low-bandwidth, latency-tolerant use cases.
- **Distributed UPF:** Edge / regional. Needed for URLLC, MEC, video, IoT.
- Slice can be: central for eMBB, edge for URLLC.

### Q28. How does GTP-U handle a UE moving cells (mid-session)?
**Answer:**
- During handover, F-TEID changes (new gNB's DL TEID).
- AMF/SMF update N3 tunnel via PFCP Session Modify.
- Source gNB sends End Marker to UPF (indicates no more data).
- New gNB's path activated.
- Sequence numbers (SN flag) used for in-order delivery.

### Q29. When would you choose DC-GBR over GBR?
**Answer:**
- **DC-GBR:** When latency budget < 50 ms AND you need guaranteed rate. URLLC scenarios. Factory automation, V2X, remote surgery.
- **GBR:** Real-time but not ultra-low latency. Voice (100 ms), video call (150 ms).

### Q30. How do you ensure security in 5GC SBA?
**Answer:**
- **HTTPS (TLS) between NFs.**
- **OAuth2** for service authorization (NF token from NRF).
- **NAS encryption** between UE and AMF.
- **PDCP encryption** between UE and gNB.
- **IPsec on N3** (optional, encrypts GTP-U).
- **SUCI** instead of SUPI over air.
- Network slice isolation.

---

## END OF LEVEL 10 (30 questions)

> Move to **11_HR_BEHAVIORAL.md**.
