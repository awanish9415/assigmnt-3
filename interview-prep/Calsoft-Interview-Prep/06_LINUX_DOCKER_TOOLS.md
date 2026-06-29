# Level 6: Linux, Docker, Wireshark, tcpdump, iproute2

> JD mentions: "Linux advanced (Ubuntu 22.04+, kernel tuning, CPU pinning), Docker, Wireshark/tcpdump, iproute2, iptables, ethtool"

---

## SECTION A: Linux Networking Basics (1-15)

### Q1. What is the difference between Ubuntu 22.04 and older versions for 5G work?
**Answer:** Ubuntu 22.04 LTS has:
- Newer kernel (5.15+, supports modern NICs).
- Better DPDK support.
- systemd-networkd improvements.
- Python 3.10 default.
- Modern OpenSSL 3.0.

### Q2. What is iproute2?
**Answer:** iproute2 is a Linux package replacing old tools (ifconfig, route, arp). Main command: `ip`. Modern and feature-rich.

### Q3. Common `ip` commands?
**Answer:**
```bash
ip link show               # Show interfaces
ip link set eth0 up        # Bring up interface
ip addr show               # Show IPs
ip addr add 10.0.0.1/24 dev eth0
ip route show              # Show routing table
ip route add 192.168.1.0/24 via 10.0.0.254
ip neigh show              # ARP table
ip -s link show eth0       # Statistics
ip netns add ns1           # Create namespace
ip netns exec ns1 ip link show
```

### Q4. What is `ifconfig` vs `ip addr`?
**Answer:** `ifconfig` is deprecated. `ip addr` is the modern replacement. Same purpose: show/manage IP addresses on interfaces.

### Q5. How to create a TUN/TAP interface?
**Answer:**
```bash
# TUN (Layer 3, IP)
sudo ip tuntap add name ogstun mode tun
sudo ip addr add 10.45.0.1/16 dev ogstun
sudo ip link set ogstun up

# TAP (Layer 2, Ethernet)
sudo ip tuntap add name tap0 mode tap
sudo ip link set tap0 up
```

### Q6. What is the difference between TUN and TAP?
**Answer:**
- **TUN:** Layer 3 device. Handles IP packets. Used by UPF in Open5GS.
- **TAP:** Layer 2 device. Handles Ethernet frames. Used by VMs, virtual switches.

### Q7. What is iptables?
**Answer:** Linux firewall tool. Filters packets and does NAT. Replaced by nftables in newer systems but still widely used.

### Q8. What is the NAT MASQUERADE rule for UPF?
**Answer:**
```bash
sudo iptables -t nat -A POSTROUTING -s 10.45.0.0/16 ! -o ogstun -j MASQUERADE
```
This takes UE traffic (10.45.0.0/16) leaving via eth0 and replaces source IP with eth0's IP. Required so packets return.

### Q9. What is IP forwarding and how to enable?
**Answer:** Allows Linux to route packets between interfaces.
```bash
sudo sysctl -w net.ipv4.ip_forward=1
echo "net.ipv4.ip_forward=1" | sudo tee -a /etc/sysctl.conf
```

### Q10. What is ethtool?
**Answer:** Tool to query/configure NIC settings:
```bash
ethtool eth0                  # Show settings
ethtool -i eth0               # Driver info
ethtool -S eth0               # Statistics
ethtool -K eth0 tso off       # Disable TSO
ethtool -G eth0 rx 4096       # Ring buffer size
ethtool -L eth0 combined 8    # Number of queues
ethtool -T eth0               # Time stamping (important for PTP)
```

### Q11. What kernel parameters tune networking?
**Answer:**
```bash
# Buffer sizes
net.core.rmem_max = 134217728
net.core.wmem_max = 134217728
net.core.netdev_max_backlog = 5000

# TCP
net.ipv4.tcp_rmem = 4096 87380 134217728
net.ipv4.tcp_wmem = 4096 65536 134217728

# Connection tracking
net.netfilter.nf_conntrack_max = 1048576
```

### Q12. What is SR-IOV?
**Answer:** SR-IOV (Single Root I/O Virtualization) divides one physical NIC into multiple virtual NICs (VFs). Each VF can be passed to a VM/container directly. Used for DPDK and high-perf networking.

### Q13. What is hugepages?
**Answer:** Linux memory pages (default 4KB) can be larger (2MB or 1GB). DPDK needs hugepages for fast packet processing.
```bash
echo 4096 > /proc/sys/vm/nr_hugepages    # 2MB pages
# Or in GRUB: default_hugepagesz=1G hugepagesz=1G hugepages=8
```

### Q14. What is CPU pinning?
**Answer:** Force a thread/process to specific CPU core. Used for real-time apps (srsRAN, DPDK).
```bash
taskset -c 2,3 ./gnb -c gnb.yml
```
Or via cgroups, or thread API (pthread_setaffinity_np).

### Q15. What is `isolcpus`?
**Answer:** Kernel boot parameter to remove CPUs from default scheduler. Apps must explicitly use them.
```
isolcpus=2,3,4,5 nohz_full=2,3,4,5 rcu_nocbs=2,3,4,5
```
In `/etc/default/grub` then `update-grub`.

---

## SECTION B: tcpdump and Wireshark (16-30)

### Q16. What is tcpdump?
**Answer:** CLI packet capture tool on Linux. Reads from interfaces, saves to pcap file.

### Q17. Common tcpdump commands?
**Answer:**
```bash
tcpdump -i eth0                          # Capture on eth0
tcpdump -i any -w out.pcap               # All interfaces, save to file
tcpdump -i eth0 host 10.0.0.1            # By IP
tcpdump -i eth0 port 38412               # By port (NGAP)
tcpdump -i eth0 sctp                     # SCTP only
tcpdump -i eth0 udp port 2152            # GTP-U
tcpdump -i eth0 -nn -X                   # Hex + ASCII, no DNS lookup
tcpdump -r capture.pcap                  # Read pcap file
tcpdump -i eth0 -c 100                   # 100 packets
```

### Q18. What is BPF filter?
**Answer:** Berkeley Packet Filter - syntax tcpdump uses. Examples:
- `tcp port 80`
- `host 10.0.0.1 and port 22`
- `not arp`
- `src net 10.45.0.0/16`

### Q19. How to capture only first 64 bytes?
**Answer:** `-s 64` snap length.
```bash
tcpdump -i eth0 -s 64 -w small.pcap
```

### Q20. How to rotate pcap files?
**Answer:**
```bash
tcpdump -i eth0 -w trace_%H%M.pcap -G 3600 -W 24
```
New file every 3600s, keep 24 files.

### Q21. What is Wireshark?
**Answer:** GUI packet analyzer. Reads pcap. Decodes 1000+ protocols including NGAP, F1AP, GTP, PFCP, SCTP, eCPRI.

### Q22. How to filter in Wireshark display?
**Answer:**
- `ngap` - NGAP messages
- `f1ap` - F1AP
- `gtp` - GTP-U
- `sctp` - SCTP
- `ip.addr == 10.0.0.1`
- `udp.port == 2152`
- `sctp.port == 38412`

### Q23. How to decode SCTP port as NGAP?
**Answer:** In Wireshark: Edit > Preferences > Protocols > SCTP. Or right-click packet > "Decode As" > NGAP.

### Q24. How does Wireshark decode NGAP and F1AP?
**Answer:** Built-in dissectors for ASN.1 PER. Recognizes by SCTP PPID (Payload Protocol Identifier):
- NGAP PPID = 60
- F1AP PPID = 62
- E1AP PPID = 64
- XnAP PPID = 61

If PPID is set, no manual decode needed.

### Q25. How to capture on remote machine?
**Answer:**
```bash
ssh user@remote 'tcpdump -i eth0 -w -' | wireshark -k -i -
```
Or save locally and copy:
```bash
ssh user@remote 'sudo tcpdump -i eth0 -w /tmp/out.pcap'
scp user@remote:/tmp/out.pcap .
```

### Q26. What is "ring buffer" in tcpdump?
**Answer:** `-W N -C size` rotates files. Keeps N files of size MB:
```bash
tcpdump -i eth0 -w cap.pcap -C 100 -W 5
# Max 5 files, each 100 MB.
```

### Q27. How to capture VLAN-tagged traffic?
**Answer:** Default tcpdump may not show VLAN. Use:
```bash
tcpdump -i eth0 -e vlan
tcpdump -i eth0 vlan 100
```

### Q28. How to filter for a specific 5G UE in NGAP?
**Answer:** Each UE has a "RAN UE NGAP ID" and "AMF UE NGAP ID". Filter:
```
ngap.RAN_UE_NGAP_ID == 1
ngap.AMF_UE_NGAP_ID == 2
```

### Q29. How to filter for GTP-U with specific TEID?
**Answer:**
```
gtp.teid == 0x12345678
```

### Q30. How to view eCPRI in Wireshark?
**Answer:** Wireshark has eCPRI dissector. Filter: `ecpri`. Look at message type (0x00 IQ data, 0x02 RT control). Plus, O-RAN U-plane has separate dissector (oran).

---

## SECTION C: Docker (31-45)

### Q31. What is Docker?
**Answer:** Docker is a container platform. Containers are like lightweight VMs but share host kernel. Used to package apps with all dependencies.

### Q32. What is the difference between VM and container?
**Answer:**
- **VM:** Full OS. Slower boot, more memory. Strong isolation.
- **Container:** Shares host kernel. Fast (seconds), less memory. Weaker isolation.

### Q33. What is a Docker image?
**Answer:** Read-only template for creating containers. Built from a `Dockerfile`.

### Q34. What is a Docker container?
**Answer:** Running instance of an image.

### Q35. Common Docker commands?
**Answer:**
```bash
docker pull ubuntu:22.04                 # Download image
docker images                            # List images
docker ps                                # Running containers
docker ps -a                             # All containers
docker run -it ubuntu:22.04 bash         # Run interactively
docker run -d --name myapp myimage       # Detached
docker exec -it myapp bash               # Enter running container
docker logs myapp                        # View logs
docker stop myapp
docker rm myapp
docker rmi myimage
docker build -t myimage:1.0 .            # Build from Dockerfile
```

### Q36. What is a Dockerfile?
**Answer:** Text file with build instructions:
```dockerfile
FROM ubuntu:22.04
RUN apt update && apt install -y build-essential cmake libsctp-dev
COPY . /app
WORKDIR /app
RUN make
CMD ["./mybinary"]
```

### Q37. What is docker-compose?
**Answer:** Tool to define and run multi-container apps using YAML:
```yaml
version: '3'
services:
  amf:
    image: open5gs/amf:latest
    ports:
      - "38412:38412/sctp"
  mongo:
    image: mongo:6.0
```

### Q38. How to deploy Open5GS in Docker?
**Answer:** Use community images (e.g., `nickvsnetworking/open5gs`) or build own:
```yaml
services:
  mongo:
    image: mongo:6.0
  nrf:
    image: open5gs:latest
    command: open5gs-nrfd
    volumes:
      - ./nrf.yaml:/etc/open5gs/nrf.yaml
  amf:
    image: open5gs:latest
    command: open5gs-amfd
    depends_on:
      - nrf
    network_mode: host    # For SCTP simplicity
```

### Q39. Why use `network_mode: host` for 5GC in Docker?
**Answer:** SCTP and complex IP setups (loopback addresses) work easier in host mode. Default Docker bridge has limitations with SCTP multi-homing.

### Q40. What is Docker bridge vs host vs none?
**Answer:**
- **bridge (default):** Container gets private IP, NAT.
- **host:** Container shares host network namespace. No isolation.
- **none:** No network.
- **macvlan:** Each container gets unique MAC on host network.

### Q41. How to give a container a TUN device?
**Answer:**
```bash
docker run --cap-add=NET_ADMIN --device=/dev/net/tun myimage
```
Plus, in compose:
```yaml
cap_add:
  - NET_ADMIN
devices:
  - /dev/net/tun:/dev/net/tun
```

### Q42. How to expose SCTP port from Docker?
**Answer:**
```yaml
ports:
  - "38412:38412/sctp"
```
Or use `network_mode: host`.

### Q43. What is Docker volume?
**Answer:** Persistent storage for containers. Survives container restart/delete.
```bash
docker run -v /host/path:/container/path
docker run -v myvol:/data
docker volume create myvol
```

### Q44. What are Docker logs and where do they go?
**Answer:** stdout/stderr of container. Stored in `/var/lib/docker/containers/<id>/`. View with `docker logs <name>`.

### Q45. How to debug a container that won't start?
**Answer:**
- `docker logs <name>` - check stderr.
- `docker inspect <name>` - config and state.
- `docker run --entrypoint sh -it myimage` - bypass entrypoint.
- `docker run -it myimage bash` - manual run.

---

## SECTION D: Linux Kernel Tuning + Misc (46-50)

### Q46. How to check if SCTP kernel module is loaded?
**Answer:**
```bash
lsmod | grep sctp
sudo modprobe sctp
```

### Q47. How to enable real-time priority?
**Answer:**
- `chrt -r -p 99 <pid>` - set real-time priority.
- `ulimit -r 99` - max RT priority.
- Use `SCHED_FIFO` or `SCHED_RR` scheduler.

### Q48. What is `numactl`?
**Answer:** NUMA (Non-Uniform Memory Access) control. Pin process to specific NUMA node:
```bash
numactl --cpunodebind=0 --membind=0 ./gnb
numactl --hardware    # Show NUMA topology
```

### Q49. How to monitor system performance for 5G?
**Answer:**
- `top`, `htop` - CPU/memory.
- `iostat`, `iotop` - disk.
- `iftop`, `nload`, `vnstat` - network bandwidth.
- `perf top` - profiling.
- `pidstat -t -p <pid>` - per-thread CPU.
- `mpstat -P ALL` - per-CPU stats.

### Q50. What is `sysctl` and important params for 5G?
**Answer:** Runtime kernel parameter management.
```bash
sysctl net.core.rmem_max
sysctl -w net.ipv4.ip_forward=1

# Persist in /etc/sysctl.conf or /etc/sysctl.d/*.conf
```

Important for 5G:
- `net.core.rmem_max`, `wmem_max` - socket buffer.
- `net.ipv4.ip_forward = 1` - routing.
- `net.netfilter.nf_conntrack_max` - high conn count.
- `kernel.sched_rt_runtime_us = -1` - allow 100% RT.

---

## END OF LEVEL 6 (50 questions)

> Move to **07_CPP_QUESTIONS.md**.
