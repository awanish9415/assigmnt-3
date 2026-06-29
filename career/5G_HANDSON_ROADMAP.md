# 5G Real Hands-On Roadmap — Training se REAL Skill tak

> Goal: 8–10 hafte mein theoretical knowledge ko **real, demonstrable skill**
> mein convert karna — taaki resume mein jhooth na bolna pade aur interview
> mein confidently bol sake. Daily 4–6 ghante de paya to yeh timeline real hai.

---

## Tu kahan khada hai (honest assessment)
- ✅ Theory strong: MAC, RLC, PDCP, RACH, HARQ padha hai (repo proof hai)
- ✅ C/C++ aata hai
- ❌ Kabhi REAL production/open-source 5G stack pe kaam nahi kiya
- ❌ End-to-end gNB/UE chala ke nahi dekha

**Fix:** OpenAirInterface (OAI) aur srsRAN — ye duniya ke do sabse bade
**open-source 5G stacks** hain. Inpe kaam = REAL experience. Resume pe likh
sakta hai, GitHub pe dikha sakta hai, interview mein bol sakta hai.

---

## TOOL CHOICE
- **srsRAN Project** → seekhne ke liye easiest, clean C++ code, accha docs.
  **Yahan se shuru kar.**
- **OpenAirInterface (OAI)** → industry mein zyada use hota hai, thoda complex.
  srsRAN ke baad.

System requirement: Ubuntu 22.04 (native ya VM), 8GB+ RAM, multi-core CPU.
RF hardware (USRP) ki zaroorat NAHI — **RFsimulator / ZMQ** se sab software
mein chalega.

---

## WEEK-BY-WEEK PLAN

### Week 1–2: srsRAN build & run (end-to-end 5G call)
- [ ] Ubuntu 22.04 setup (VM chalega)
- [ ] srsRAN Project ko source se build kar (cmake, make)
- [ ] Open5GS core network setup kar (5G core)
- [ ] srsRAN gNB + srsUE ko **ZMQ/RFsim** pe connect kar — bina hardware
- [ ] Ek successful **end-to-end attach + RACH + data** dekh
- 🎯 Outcome: "Maine real 5G gNB-UE call chalaya, RACH logs dekhe"

### Week 3–4: Code padho & samjho (MAC focus)
- [ ] srsRAN ka **MAC layer code** padh (sched, RACH handling, HARQ)
- [ ] GDB se breakpoint laga ke RACH Msg1–Msg4 flow trace kar
- [ ] Wireshark + srsRAN logs se signalling capture kar
- [ ] Apne notes likh: "yeh function kya karta hai" (repo mein add kar)
- 🎯 Outcome: real codebase navigate karna aata hai

### Week 5–6: Modify / Experiment (yeh sabse important)
- [ ] Ek chhota change kar: logging add kar, ek scheduler param tweak kar,
      ya ek config option add kar
- [ ] Build kar, test kar, before/after behaviour compare kar
- [ ] Apne fork pe commit kar with clean git history
- [ ] (Bonus) srsRAN/OAI GitHub issues padh — ek "good first issue" attempt kar
- 🎯 Outcome: "Maine real 5G stack modify kiya" — yeh interview gold hai

### Week 7–8: OpenAirInterface + Portfolio
- [ ] OAI gNB build & run kar (RFsim mode)
- [ ] srsRAN vs OAI architecture difference samajh
- [ ] GitHub portfolio bana:
      - apna RACH simulator (clean up + README)
      - srsRAN/OAI experiments + notes
      - architecture diagrams
- [ ] Resume update kar: "OAI/srsRAN hands-on" section add kar
- 🎯 Outcome: demonstrable, honest, real portfolio

---

## RESOURCES (free)
- **srsRAN Project docs:** docs.srsran.com
- **OpenAirInterface:** gitlab.com/oai/openairinterface5g (official repo)
- **Open5GS docs:** open5gs.org/open5gs/docs
- **3GPP TS 38.321** (tera repo mein already notes hain — revise karte reh)
- YouTube: "srsRAN 5G setup", "OpenAirInterface tutorial" search kar

---

## PARALLEL: Job + Income (roadmap chalte hue)
1. **Honest resume** (career/RESUME_AWANISH_HONEST.md) se abhi apply karna shuru kar
2. Roles: "5G L2 simulator dev", "protocol test engineer", "telecom R&D C/C++"
3. GPON real experience wali jobs bhi parallel apply kar (Tejas, STL, HFCL)
4. Income gap ho to: freelance (embedded C/C++), telecom training/mentoring
5. Har hafte 10–15 targeted applications (LinkedIn + company career pages)

---

## INTERVIEW HONESTY SCRIPT (yaad rakh)
Jab poochein "production experience?" — aise bol:
> "Mera core kaam simulator aur POC level pe tha — maine 3GPP TS 38.321 ke
> hisaab se MAC/RACH/HARQ implement kiya. Production deployment pe nahi tha,
> lekin maine fundamentals deeply samjhe hain, aur ab srsRAN/OAI jaise real
> open-source stacks pe hands-on kar raha hu. Yeh raha mera GitHub."

Yeh honesty + initiative = interviewer impress hota hai. Jhooth se ulta sab
khatam ho jaata hai.

---

## SUCCESS METRIC (8 hafte baad)
- [ ] srsRAN end-to-end call chalaya ✅
- [ ] MAC code modify kiya + GitHub pe ✅
- [ ] OAI build kiya ✅
- [ ] Honest resume + GitHub portfolio ready ✅
- [ ] 50+ targeted applications bheje ✅
- [ ] Confidently, honestly interview de sakta hai ✅
