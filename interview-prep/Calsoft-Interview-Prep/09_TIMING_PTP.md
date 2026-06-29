# Level 9: Timing — ptp4l, phc2sys, G.8275.1

> JD: "Fronthaul timing setup (ptp4l/phc2sys)". This is critical for O-RAN. Expect 5-8 questions on this.

---

## SECTION A: Why Timing Matters (1-10)

### Q1. Why is timing critical in 5G fronthaul?
**Answer:** 
- TDD requires UL and DL on same frequency at different times → all cells must agree on time.
- O-RAN 7.2 fronthaul has strict timing windows (microseconds).
- Inter-cell interference if not synced.
- Carrier Aggregation needs sync.
- Beamforming, MIMO need sync between antennas.

### Q2. What sync accuracy is needed in 5G?
**Answer:**
- **Frequency:** ±50 ppb (parts per billion).
- **Phase:** ±1.5 µs (for TDD).
- **Time of Day:** ±1.5 µs.
- For fronthaul (RU): even tighter, ±100 ns class A, ±200 ns class B.

### Q3. What is PTP?
**Answer:** PTP (Precision Time Protocol) - IEEE 1588 standard. Distributes time over Ethernet with sub-microsecond accuracy. Used in telecom, finance, industrial.

### Q4. What is the difference between PTP and NTP?
**Answer:**
- **NTP:** Software-based. ~ms accuracy. Good for general computing.
- **PTP:** Hardware-assisted (timestamping in NIC). ~ns accuracy. Required for telecom.

### Q5. What is SyncE?
**Answer:** SyncE (Synchronous Ethernet) - frequency sync via Ethernet physical layer. Like a stable clock signal. Used alongside PTP for frequency stability.

### Q6. What is G.8275.1?
**Answer:** ITU-T standard. PTP profile for telecom with full timing support from network. Strict hop-by-hop synchronization. All switches must support PTP (Boundary Clock).

### Q7. What is G.8275.2?
**Answer:** PTP profile with **partial** timing support. Some switches don't support PTP. Less strict. Used when full G.8275.1 not possible.

### Q8. What is the difference between G.8275.1 and 8275.2?
**Answer:**
- **G.8275.1:** Full timing support (FTS). Multicast L2 PTP. All hops sync.
- **G.8275.2:** Partial timing support (PTS). Unicast L3 PTP. Some non-PTP hops.

### Q9. What is GM (Grandmaster)?
**Answer:** GM is the ultimate time source in PTP. Usually has GPS receiver. All clocks in network synchronize to GM.

### Q10. What is BMCA?
**Answer:** BMCA (Best Master Clock Algorithm) selects the best clock to be GM. Considers priority, clock class, accuracy.

---

## SECTION B: PTP Clock Types (11-15)

### Q11. What is OC, BC, TC?
**Answer:**
- **OC (Ordinary Clock):** Endpoint. Either GM (master) or slave (sync to master).
- **BC (Boundary Clock):** Switch/router with PTP. Acts as slave on one port, master on others. Re-times.
- **TC (Transparent Clock):** Doesn't re-time. Just measures residence time and adds to PTP message.

### Q12. What clock is used in 5G fronthaul?
**Answer:** **T-BC (Telecom Boundary Clock)** in switches. **T-GM** at top. **T-TSC (T-Time Slave Clock)** at end (e.g., O-RU).

### Q13. What is a Telecom Profile?
**Answer:** ITU-T defined profile of PTP for telecom. Specifies:
- Multicast/unicast.
- L2/L3.
- Message rates.
- Clock classes.
- BMCA modification.

### Q14. What is the message rate in PTP?
**Answer:** Typical:
- Sync: 16 msg/s (every 62.5 ms).
- Announce: 1 msg/s.
- Delay_Req/Delay_Resp: 16/s.

### Q15. What are PTP message types?
**Answer:**
- **Sync:** Master sends timestamp.
- **Follow_Up:** If 2-step, contains precise TX timestamp.
- **Delay_Req:** Slave asks master for delay measurement.
- **Delay_Resp:** Master responds.
- **Announce:** Master announces itself (for BMCA).
- **Pdelay_Req/Resp/Resp_Follow_Up:** Peer delay mechanism (P2P TC).

---

## SECTION C: ptp4l and phc2sys (16-25)

### Q16. What is ptp4l?
**Answer:** Linux PTP daemon (part of linuxptp package). Runs PTP protocol, syncs **PHC (PTP Hardware Clock)** of NIC to master.

### Q17. What is phc2sys?
**Answer:** Sync between **PHC** (NIC hardware clock) and **system clock** (CLOCK_REALTIME). Either direction:
- PHC -> sys: Sys clock follows PHC (when PHC is best).
- Sys -> PHC: PHC follows sys (rarely).

### Q18. Why both ptp4l and phc2sys?
**Answer:** 
- ptp4l syncs NIC PHC to GM (network time).
- phc2sys syncs Linux system clock to PHC.
- Both needed so software (incl. user apps) see correct time.

### Q19. Sample ptp4l config?
**Answer:**
```
[global]
domainNumber              24
priority1                 128
priority2                 128
clockClass                248
clockAccuracy             0xfe
offsetScaledLogVariance   0xffff
free_running              0
freq_est_interval         1
delay_mechanism           E2E
network_transport         L2
time_stamping             hardware
slaveOnly                 1
[eno1]
logAnnounceInterval       0
logSyncInterval           -4    # 16 msg/s
logMinDelayReqInterval    -4
```

### Q20. Run ptp4l command?
**Answer:**
```bash
sudo ptp4l -f /etc/ptp4l.conf -i eno1 -m
# -m = print to stdout
# -i = interface
```

### Q21. Run phc2sys command?
**Answer:**
```bash
# Sync system clock to PHC
sudo phc2sys -s eno1 -O 0 -m

# -s source clock
# -O offset (usually 0 in linux)
```

### Q22. How to check if NIC supports PTP hw timestamping?
**Answer:**
```bash
ethtool -T eno1
# Look for:
# hardware-transmit-timestamping
# hardware-receive-timestamping
# PTP Hardware Clock: 0
```

### Q23. How to view PHC time?
**Answer:**
```bash
testptp -d /dev/ptp0 -g     # Get PHC time
```

### Q24. Common ptp4l logs?
**Answer:**
```
ptp4l[123]: selected /dev/ptp0 as PTP clock
ptp4l[123]: port 1: INITIALIZING to LISTENING
ptp4l[123]: port 1: LISTENING to UNCALIBRATED
ptp4l[123]: master offset       -120 s2 freq +1234 path delay  -456
ptp4l[123]: port 1: UNCALIBRATED to SLAVE
```
"s2" means in SLAVE state. Offset should be small (ns).

### Q25. How to integrate PTP with O-RAN?
**Answer:**
- O-RU has PTP slave (T-TSC).
- O-DU has PTP slave or master, often T-BC.
- Network switches must be PTP-aware (T-BC).
- GM provides absolute time (PRTC class A or B).
- All under G.8275.1 profile typically.

---

## END OF LEVEL 9 (25 questions)

> Move to **10_SCENARIOS_TROUBLESHOOT.md**.
