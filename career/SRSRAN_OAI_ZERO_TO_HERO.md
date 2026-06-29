# srsRAN + OpenAirInterface — Zero se Hero tak (Hinglish Guide)

> Bhai, yeh guide bilkul ZERO se hai. Har command copy-paste karne layak hai.
> Koi RF hardware (USRP/radio) ki zaroorat NAHI — sab kuch software simulator
> (ZMQ / RFsimulator) pe chalega. Sirf ek Ubuntu machine chahiye.
>
> Plan: pehle **srsRAN** (easy, clean code) seekhega, end-to-end 5G call
> chalayega. Phir **OpenAirInterface (OAI)** karega (industry favourite).
> Aakhir mein ek **real project** banayega jo resume + GitHub pe jayega.

---

## PHASE 0 — Machine Setup (Day 0)

### Kya chahiye
- **Ubuntu 22.04 LTS** (sabse important — yahi use kar, 24.04 pe kabhi-kabhi dikkat)
- 8GB+ RAM (16GB best), 4+ CPU cores, 40GB+ free disk
- Internet

### Ubuntu kahan chalaye? (3 options)
1. **Native install** (best performance) — agar spare laptop hai
2. **VirtualBox / VMware VM** (easiest start) — Windows/Mac pe Ubuntu VM bana
3. **WSL2 (Windows)** — chal jata hai but networking thoda extra setup maangta

> Recommendation: shuruaat ke liye **VirtualBox + Ubuntu 22.04 VM** sabse
> simple hai. Baad mein native pe shift kar sakta hai.

### Base tools install (Ubuntu terminal mein)
```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y git cmake make gcc g++ pkg-config \
    libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev \
    libgtest-dev libzmq3-dev build-essential net-tools \
    iproute2 iptables tcpdump wireshark gdb valgrind
```

---

# ════════════════════════════════════════
# PART 1 — srsRAN (5G end-to-end, ZMQ)
# ════════════════════════════════════════

> Architecture: **Core (Open5GS)**  ⇄  **gNB (srsRAN Project)**  ⇄
> **UE (srsRAN 4G ka srsUE)** — gNB aur UE ke beech radio ki jagah **ZMQ**
> (software virtual radio) use hoga.

## STEP 1.1 — srsRAN Project (gNB) build (Week 1)

```bash
cd ~
git clone https://github.com/srsRAN/srsRAN_Project.git
cd srsRAN_Project
mkdir build && cd build
cmake ../ -DENABLE_EXPORT=ON -DENABLE_ZEROMQ=ON
make -j $(nproc)
sudo make install
sudo ldconfig
```
> ⚠️ Build mein `-DENABLE_ZEROMQ=ON` ZAROORI hai — yahi hardware-free testing
> deta hai. Build ke baad log mein "FINDING ZeroMQ ... found" check kar.

Test ki build sahi hai:
```bash
./apps/gnb/gnb --version
```

## STEP 1.2 — srsRAN 4G (srsUE ke liye) build

srsRAN Project mein UE nahi aata, isliye purana srsRAN 4G chahiye srsUE ke liye:
```bash
cd ~
git clone https://github.com/srsRAN/srsRAN_4G.git
cd srsRAN_4G
mkdir build && cd build
cmake ../ -DENABLE_ZEROMQ=ON
make -j $(nproc)
sudo make install
sudo ldconfig
```

## STEP 1.3 — Open5GS (5G Core) install (Week 1)

5G Core network. Easiest = official apt repo:
```bash
sudo apt install -y software-properties-common
sudo add-apt-repository -y ppa:open5gs/latest
sudo apt update
sudo apt install -y open5gs
```
Plus WebUI (subscriber add karne ke liye) — Node.js chahiye:
```bash
sudo apt install -y curl
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo bash -
sudo apt install -y nodejs
curl -fsSL https://open5gs.org/open5gs/assets/webui/install | sudo -E bash -
```
WebUI: browser mein `http://localhost:9999` (login: admin / 1423).

## STEP 1.4 — Subscriber (UE) add kar Open5GS WebUI mein

WebUI mein ek subscriber daal. srsRAN ki default test values use kar:
- **IMSI:** 001010123456780
- **Key (K):** 00112233445566778899aabbccddeeff
- **OPc:** 63bfa50ee6523365ff14c1f45f88737d
- Baaki default chhod de.

## STEP 1.5 — Sab chala (3 terminals)

**Terminal 1 — Core:**
```bash
sudo systemctl restart open5gs-*    # saare core NFs start
# (ya manually: sudo open5gs-amfd, open5gs-upfd, etc.)
```

**Terminal 2 — gNB (ZMQ config ke saath):**
```bash
cd ~/srsRAN_Project
sudo ./build/apps/gnb/gnb -c configs/gnb_zmq.yaml
```
> Repo ke `configs/` folder mein ZMQ wale example configs milenge. Agar
> `gnb_zmq.yaml` na ho to docs ka "srsUE with ZMQ" tutorial config copy kar.

**Terminal 3 — srsUE:**
```bash
cd ~/srsRAN_4G
sudo ./build/srsue/src/srsue ./srsue/ue.conf.example   # ZMQ rf settings ke saath
```

### Success kaise pata chale? 🎉
- gNB log: "RACH" / "UE attached" type messages
- srsUE: IP address milega (e.g. 10.45.0.x)
- Test internet (UE ke namespace se):
```bash
ping -I tun_srsue 8.8.8.8     # interface name config pe depend karega
```

> 🏆 Yahan tak pahunch gaya to tu ne **real 5G end-to-end call** chala li,
> bina hardware. Yeh bahut log nahi kar paate. Screenshot le, GitHub README
> mein daal.

---

## STEP 1.6 — Code padho (Week 3) — MAC focus

srsRAN Project ka folder structure:
```
srsRAN_Project/
├── lib/
│   ├── mac/          ← YAHAN tera focus (MAC layer)
│   ├── rlc/          ← RLC
│   ├── pdcp/         ← PDCP
│   ├── scheduler/    ← MAC scheduler (bahut important)
│   └── ...
├── apps/gnb/         ← main entry point
└── configs/          ← config files
```

Kaam:
- [ ] `lib/mac/` aur `lib/scheduler/` ki files khol, padh
- [ ] RACH handling code dhoondh (grep kar: `grep -ri "rach" lib/mac/`)
- [ ] GDB se gNB ko breakpoint pe rok ke RACH flow trace kar:
```bash
sudo gdb --args ./build/apps/gnb/gnb -c configs/gnb_zmq.yaml
# (gdb) break <rach handler function>
# (gdb) run
```
- [ ] Apne notes likh (tere repo ke L2-MAC-Telecom notes se match kar)

---

## STEP 1.7 — MODIFY kar (Week 5) — yeh sabse important

Chhota change → build → behaviour compare. Examples:
- [ ] MAC scheduler mein extra logging add kar (kaunsa UE kab schedule hua)
- [ ] Ek scheduler parameter tweak kar, throughput pe asar dekh
- [ ] RACH ke ek timer/param ka log add kar

```bash
# change ke baad sirf rebuild:
cd ~/srsRAN_Project/build && make -j $(nproc)
```
Apne **fork** pe commit kar (GitHub pe apna srsRAN fork bana ke):
```bash
git checkout -b feature/mac-extra-logging
git add -A && git commit -m "Add MAC scheduler debug logging for UE grants"
git push origin feature/mac-extra-logging
```
> 🎯 "Maine srsRAN ke MAC scheduler mein change kiya, build kiya, test kiya"
> — yeh sentence interview mein GOLD hai.

---

# ════════════════════════════════════════
# PART 2 — OpenAirInterface (OAI)
# ════════════════════════════════════════

> OAI industry mein zyada use hota hai (Nokia, operators, O-RAN). srsRAN
> seekhne ke baad isko kar — RFsimulator mode mein, bina hardware.

## STEP 2.1 — OAI 5G Core (CN5G) — Docker se (easiest)

```bash
sudo apt install -y docker.io docker-compose-v2
sudo usermod -aG docker $USER     # logout/login ke baad effect
cd ~
git clone https://github.com/OPENAIRINTERFACE/openairinterface5g.git
cd openairinterface5g/doc
# Tutorial follow kar: NR_SA_Tutorial_OAI_CN5G.md
```
CN5G docker compose se uth jata hai (AMF/SMF/UPF containers).

## STEP 2.2 — OAI gNB + nrUE build (RFsimulator)

```bash
cd ~/openairinterface5g
source oaienv
cd cmake_targets
./build_oai -I              # pehli baar: dependencies install
./build_oai -w SIMU --gNB --nrUE --ninja
```
> `-w SIMU` = RFsimulator (no hardware). `--gNB` aur `--nrUE` dono build hote.
> `build_oai help` se saare options dekh sakta hai.

## STEP 2.3 — gNB + nrUE chala (RFsim)

**Terminal A — gNB:**
```bash
cd ~/openairinterface5g/cmake_targets/ran_build/build
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.conf --rfsim --sa
```

**Terminal B — nrUE:**
```bash
cd ~/openairinterface5g/cmake_targets/ran_build/build
sudo ./nr-uesoftmodem --rfsim --sa -r 106 --numerology 1 -C 3619200000 \
    --uicc0.imsi 001010000000001
```
> IMSI/key ko OAI CN5G ke subscriber database se match karna padega.

### Success:
- nrUE attach hoga, `oaitun_ue1` interface banega, IP milega.
- `ping` se data test kar.

## STEP 2.4 — OAI code explore
```
openairinterface5g/
├── openair2/
│   ├── LAYER2/NR_MAC_gNB/    ← gNB MAC (tera focus)
│   ├── LAYER2/NR_MAC_UE/     ← UE MAC
│   ├── RLC/  PDCP/
│   └── ...
├── openair1/                 ← PHY layer
└── openair3/                 ← RRC, NAS, core interfaces
```

---

# ════════════════════════════════════════
# PART 3 — REAL PROJECT (resume ke liye)
# ════════════════════════════════════════

Inme se ek choose kar aur GitHub pe achhe README ke saath daal:

### Project A (Beginner): "5G SA Lab — srsRAN + Open5GS (ZMQ)"
- End-to-end setup automate kar (scripts), document kar, logs/screenshots
- README: architecture diagram, step-by-step, RACH/attach logs explained
- **Resume line:** "Built end-to-end 5G SA network (srsRAN gNB + Open5GS core)
  in ZMQ simulation; analysed RACH/attach signalling."

### Project B (Intermediate): "MAC Scheduler Instrumentation"
- srsRAN MAC scheduler mein detailed logging/metrics add kar
- UE grants, BSR, throughput ko log/plot kar (Python se graph bana)
- **Resume line:** "Instrumented srsRAN MAC scheduler; logged & visualised
  UL/DL grant allocation and BSR handling."

### Project C (Advanced): "RACH Analysis: my simulator vs real stack"
- Tera apna RACH simulator (jo repo mein hai) ko srsRAN/OAI ke real RACH
  flow se compare kar — Msg1-Msg4 timing, backoff, contention
- **Resume line:** "Compared custom RACH simulator against srsRAN/OAI MAC
  RACH implementation per 3GPP TS 38.321."

> Koi bhi project → clean README + diagrams + commit history = REAL,
> demonstrable skill. Yahi interview mein dikhana.

---

# ════════════════════════════════════════
# TROUBLESHOOTING (common dikkat)
# ════════════════════════════════════════

| Problem | Fix |
|---|---|
| ZMQ "not found" build mein | `sudo apt install libzmq3-dev`, phir `-DENABLE_ZEROMQ=ON` se rebuild |
| UE attach nahi hota | IMSI/Key/OPc core ke subscriber se exactly match karo |
| Core start nahi | `sudo systemctl status open5gs-amfd` se logs dekh, MongoDB chal raha? |
| gNB-UE connect nahi (ZMQ) | dono ke ZMQ tx/rx ports (tx_port/rx_port) cross-match hone chahiye |
| Build slow / hang | RAM kam hai — `make -j2` use kar, swap badha |
| ping fail | sahi tun interface name use kar, IP forwarding/NAT check kar |

Logs dekhna seekhna sabse important skill hai — `journalctl`, gNB/UE console,
aur Wireshark (loopback + ZMQ traffic).

---

# ════════════════════════════════════════
# 8-WEEK SCHEDULE (recap)
# ════════════════════════════════════════
- **W1-2:** srsRAN + Open5GS build, end-to-end ZMQ call ✅
- **W3-4:** srsRAN MAC/scheduler code padh + GDB trace
- **W5-6:** srsRAN modify + fork pe commit (Project B start)
- **W7:** OAI build + RFsim call
- **W8:** Project finalize + GitHub README + resume update

# RESOURCES
- srsRAN docs: https://docs.srsran.com
- srsRAN Project repo: https://github.com/srsRAN/srsRAN_Project
- srsRAN 4G repo: https://github.com/srsRAN/srsRAN_4G
- OAI repo: https://github.com/OPENAIRINTERFACE/openairinterface5g
- OAI 5G SA tutorial: openairinterface5g/doc/NR_SA_Tutorial_OAI_CN5G.md
- Open5GS docs: https://open5gs.org/open5gs/docs/
- Helper repo (srsRAN 5G + ZMQ): https://github.com/s5uishida/build_srsran_5g_zmq

> Content rephrased/summarised; verify exact commands against the latest
> official docs above (versions change). Tu ek-ek step kar, atak jaaye to
> mujhe error paste kar — main debug kara dunga. 💪
