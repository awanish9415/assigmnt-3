# IPsec Complete Guide - Interview Preparation

## Table of Contents
1. [What is IPsec?](#what-is-ipsec)
2. [IPsec Architecture](#ipsec-architecture)
3. [IPsec Protocols](#ipsec-protocols)
4. [Security Associations (SA)](#security-associations)
5. [IKE (Internet Key Exchange)](#ike-internet-key-exchange)
6. [Modes of Operation](#modes-of-operation)
7. [Cryptographic Algorithms](#cryptographic-algorithms)
8. [Packet Processing](#packet-processing)
9. [Common Interview Questions](#common-interview-questions)
10. [Real-World Scenarios](#real-world-scenarios)

---

## What is IPsec?

**IPsec (Internet Protocol Security)** is a protocol suite for securing IP communications by authenticating and encrypting each IP packet in a communication session.

### Key Features:
- **Layer 3 Protection**: Works at network layer (transparent to applications)
- **Confidentiality**: Encryption of payload
- **Integrity**: Detection of data tampering
- **Authentication**: Verification of sender identity
- **Anti-Replay**: Protection against replay attacks

### Use Cases:
- Site-to-site VPNs (connecting offices)
- Remote access VPNs (employees to corporate network)
- Securing network traffic between servers
- Cloud connectivity (AWS VPN, Azure VPN Gateway)

---

## IPsec Architecture

### Three Main Components:

#### 1. Security Protocols
- **ESP (Encapsulating Security Payload)** - Protocol 50
- **AH (Authentication Header)** - Protocol 51

#### 2. Security Association (SA)
- Defines security parameters between two peers
- One-way relationship (need 2 for bidirectional communication)

#### 3. Key Management
- **IKE (Internet Key Exchange)** - Automated key negotiation
- Manual key configuration (rare, not scalable)

### IPsec Databases:

#### Security Policy Database (SPD)
- Defines WHAT traffic should be protected
- Rules match: source IP, dest IP, port, protocol
- Actions: BYPASS, DISCARD, PROTECT

```
Example SPD Entry:
Source: 192.168.1.0/24
Dest: 10.0.0.0/8
Protocol: Any
Action: PROTECT with IPsec
```

#### Security Association Database (SAD)
- Defines HOW traffic should be protected
- Contains active SAs with keys and algorithms
- Indexed by: SPI, Destination IP, Protocol

```
Example SAD Entry:
SPI: 0x12345678
Dest IP: 10.0.0.1
Protocol: ESP
Encryption: AES-256-CBC
Auth: HMAC-SHA256
Keys: [encryption key] [auth key]
Sequence: 1024
Lifetime: 3600 seconds
```

---

## IPsec Protocols

### ESP (Encapsulating Security Payload) - Most Common

#### ESP Packet Structure:
```
+------------------+
| IP Header        | ← New IP header (Tunnel mode only)
+------------------+
| ESP Header       | ← SPI (4B) + Sequence (4B)
+------------------+
|                  | ↓
| Payload          | ← ENCRYPTED
| (Original packet)|
|                  | ↑
+------------------+
| ESP Trailer      | ← Padding + Pad Length + Next Header
+------------------+
| ICV              | ← Integrity Check Value (HMAC)
+------------------+

Authenticated: ESP Header → ICV (not including ICV itself)
Encrypted: Payload + ESP Trailer
```

#### ESP Provides:
✅ Confidentiality (encryption)  
✅ Authentication (optional but recommended)  
✅ Integrity  
✅ Anti-replay protection  

### AH (Authentication Header) - Rarely Used

#### AH Packet Structure:
```
+------------------+
| IP Header        |
+------------------+
| AH Header        | ← Next Header, Length, SPI, Sequence
+------------------+
| ICV              | ← Integrity Check Value
+------------------+
| Original Payload |
+------------------+

Authenticated: Entire packet including IP header (with mutable fields zeroed)
```

#### AH Provides:
✅ Authentication  
✅ Integrity  
✅ Anti-replay protection  
❌ NO Confidentiality (no encryption)  

### ESP vs AH Comparison:

| Feature | ESP | AH |
|---------|-----|-----|
| Encryption | ✅ Yes | ❌ No |
| Authentication | ✅ Yes | ✅ Yes |
| Integrity | ✅ Yes | ✅ Yes |
| Anti-replay | ✅ Yes | ✅ Yes |
| NAT Compatible | ✅ Yes (with NAT-T) | ❌ No |
| IP Header Protected | ❌ No | ✅ Yes |
| Protocol Number | 50 | 51 |
| Usage | Common | Rare |

**Why AH is rarely used:**
- Doesn't work with NAT (authenticates IP header)
- Most scenarios need encryption anyway
- ESP can provide authentication too

---

## Security Associations (SA)

### What is an SA?

A **Security Association** is a one-way relationship between sender and receiver that defines:
- Encryption algorithm and key
- Authentication algorithm and key
- Sequence number counter
- Lifetime (time or data based)
- Mode (transport or tunnel)
- Anti-replay window size

### SA Parameters:

```
Security Association:
├── SPI (Security Parameter Index): 0x12345678
├── Destination IP: 10.0.0.1
├── Protocol: ESP or AH
├── Mode: Transport or Tunnel
├── Encryption Algorithm: AES-256-CBC
├── Encryption Key: [32 bytes]
├── Authentication Algorithm: HMAC-SHA256
├── Authentication Key: [32 bytes]
├── Sequence Number: Current = 1024
├── Sequence Counter: Window = 64
├── Lifetime: 3600 seconds OR 100MB
└── Path MTU: 1500 bytes
```

### Bidirectional Communication:

Need **TWO** SAs for bidirectional communication:

```
Host A ←→ Host B

Outbound SA (A→B):
  SPI: 0x11111111
  Dest: B's IP
  Keys: Set 1

Inbound SA (B→A):
  SPI: 0x22222222
  Dest: A's IP
  Keys: Set 2
```

### SA Lifetime:

SAs expire based on:
- **Time**: e.g., 1 hour
- **Data**: e.g., 100 MB transferred
- **Manual deletion**

Before expiry, IKE negotiates new SA (soft lifetime).  
At expiry, SA is deleted (hard lifetime).

---

## IKE (Internet Key Exchange)

### What is IKE?

**IKE** is the protocol for:
- Authenticating peers
- Negotiating security parameters
- Establishing SAs
- Key generation and distribution

### IKE Versions:

#### IKEv1 (Legacy - RFC 2409)
- Complex, multiple modes
- Main Mode (6 messages)
- Aggressive Mode (3 messages)
- Less efficient

#### IKEv2 (Modern - RFC 7296)
- Simplified
- Always 4 messages (2 round trips)
- Built-in NAT-T
- MOBIKE (mobility support)
- Better DoS protection
- **Recommended for new deployments**

### IKEv2 Handshake:

```
Initiator                    Responder
---------                    ---------

IKE_SA_INIT (Request) ────────────►
  - SA proposals (crypto algorithms)
  - Key Exchange (DH)
  - Nonce

                      ◄──────────── IKE_SA_INIT (Response)
                                     - Chosen SA
                                     - Key Exchange (DH)
                                     - Nonce

[IKE SA established, keys derived]

IKE_AUTH (Request) ───────────────►
  - Identification
  - Authentication (PSK or cert)
  - SA proposals for CHILD_SA
  - Traffic Selectors

                      ◄──────────── IKE_AUTH (Response)
                                     - Identification
                                     - Authentication
                                     - CHILD_SA
                                     - Traffic Selectors

[CHILD_SA established - ready for ESP traffic]
```

### Two Types of SAs in IKE:

#### 1. IKE SA (ISAKMP SA)
- **Purpose**: Protects IKE messages
- **Protocol**: UDP port 500 (or 4500 with NAT-T)
- **Lifetime**: Long (typically 24 hours)
- **Used for**: Key exchange, rekeying

#### 2. CHILD SA (IPsec SA)
- **Purpose**: Protects actual data traffic
- **Protocol**: ESP (50) or AH (51)
- **Lifetime**: Short (typically 1 hour)
- **Used for**: Encrypting/authenticating IP packets

### Authentication Methods:

1. **Pre-Shared Key (PSK)**
   - Simple
   - Both sides have same secret
   - Good for site-to-site
   - Doesn't scale well

2. **RSA Signatures (Certificates)**
   - Uses PKI (Public Key Infrastructure)
   - More secure
   - Scalable
   - Enterprise deployments

3. **EAP (Extensible Authentication Protocol)**
   - For remote access VPNs
   - Integrates with RADIUS, Active Directory
   - Supports username/password

### Perfect Forward Secrecy (PFS):

- Each session uses unique keys
- Compromise of long-term keys doesn't compromise past sessions
- Uses Diffie-Hellman for ephemeral keys
- Enabled by default in modern IKE

---

## Modes of Operation

### Transport Mode

**Use Case**: Host-to-host communication

```
Original Packet:
[ IP Header ] [ TCP/UDP ] [ Payload ]

After ESP Transport Mode:
[ Original IP ] [ ESP Header ] [ Encrypted: TCP/UDP + Payload ] [ ESP Trailer ] [ ICV ]
     ↑                                          ↑
  Visible                               Encrypted
```

**Characteristics:**
- Only payload encrypted
- IP header NOT encrypted (source/dest visible)
- Less overhead
- Doesn't work well with NAT
- Used for end-to-end security

**Example:**
```
Server A (192.168.1.10) ←→ Server B (192.168.1.20)
Direct secure communication
```

### Tunnel Mode

**Use Case**: Site-to-site VPN, Remote access VPN

```
Original Packet:
[ Original IP ] [ TCP/UDP ] [ Payload ]

After ESP Tunnel Mode:
[ New IP ] [ ESP Header ] [ Encrypted: Original IP + TCP/UDP + Payload ] [ ESP Trailer ] [ ICV ]
    ↑                                        ↑
Gateway IPs                         Entire original packet encrypted
```

**Characteristics:**
- Entire original packet encrypted
- New IP header added (gateway addresses)
- More overhead
- Works with NAT
- Hides original topology
- Used for VPN gateways

**Example:**
```
Office A Network           Internet            Office B Network
(10.0.0.0/24)                                 (192.168.0.0/24)
      |                                             |
Gateway A ──────── IPsec Tunnel ─────────── Gateway B
(1.2.3.4)                                   (5.6.7.8)

New IP header: 1.2.3.4 → 5.6.7.8
Encrypted inside: 10.0.0.5 → 192.168.0.10
```

### Comparison Table:

| Aspect | Transport Mode | Tunnel Mode |
|--------|---------------|-------------|
| **Encryption** | Payload only | Entire packet |
| **IP Header** | Original preserved | New header added |
| **Overhead** | ~50 bytes | ~70 bytes |
| **Use Case** | Host-to-host | Gateway-to-gateway |
| **NAT Friendly** | ❌ Difficult | ✅ Yes |
| **Topology Hiding** | ❌ No | ✅ Yes |
| **Common Usage** | Rare | Very common |

---

## Cryptographic Algorithms

### Encryption Algorithms:

| Algorithm | Key Size | Block Size | Speed | Security | Status |
|-----------|----------|------------|-------|----------|--------|
| **DES** | 56-bit | 64-bit | Fast | ❌ Broken | Deprecated |
| **3DES** | 168-bit | 64-bit | Slow | ⚠️ Weak | Legacy |
| **AES-128** | 128-bit | 128-bit | Fast | ✅ Strong | Recommended |
| **AES-256** | 256-bit | 128-bit | Fast | ✅ Very Strong | Recommended |
| **ChaCha20** | 256-bit | Stream | Very Fast | ✅ Strong | Modern |

**Cipher Modes:**
- **CBC** (Cipher Block Chaining): Traditional, needs IV
- **CTR** (Counter): Parallel processing possible
- **GCM** (Galois/Counter Mode): AEAD (encryption + auth combined)

### Authentication Algorithms:

| Algorithm | Output Size | Speed | Security | Status |
|-----------|-------------|-------|----------|--------|
| **HMAC-MD5** | 128-bit | Fast | ❌ Weak | Deprecated |
| **HMAC-SHA1** | 160-bit | Fast | ⚠️ Weak | Legacy |
| **HMAC-SHA256** | 256-bit | Fast | ✅ Strong | Recommended |
| **HMAC-SHA384** | 384-bit | Medium | ✅ Strong | Recommended |
| **HMAC-SHA512** | 512-bit | Medium | ✅ Strong | Recommended |

### Diffie-Hellman Groups:

| Group | Algorithm | Key Size | Security | Usage |
|-------|-----------|----------|----------|-------|
| **2** | MODP | 1024-bit | ❌ Weak | Deprecated |
| **5** | MODP | 1536-bit | ⚠️ Acceptable | Legacy |
| **14** | MODP | 2048-bit | ✅ Good | Common |
| **19** | ECC (P-256) | 256-bit | ✅ Strong | Recommended |
| **20** | ECC (P-384) | 384-bit | ✅ Very Strong | High Security |

**Modern Recommendation:**
```
Encryption: AES-256-GCM
Authentication: (built into GCM)
DH Group: 19 or 20
PRF: HMAC-SHA256 or better
```

---

## Packet Processing

### Outbound Processing (Sending):

```
1. Application sends data
   ↓
2. Check SPD (Security Policy Database)
   - Match: src IP, dst IP, port, protocol
   - Action: BYPASS, DISCARD, or PROTECT
   ↓
3. If PROTECT, lookup SA in SAD
   - Find matching outbound SA
   ↓
4. Increment sequence number
   ↓
5. Encrypt payload
   - Use encryption algorithm and key from SA
   ↓
6. Add ESP header (SPI, sequence)
   ↓
7. Add padding (if needed for block cipher)
   ↓
8. Add ESP trailer
   ↓
9. Calculate ICV (authentication)
   ↓
10. Add new IP header (if tunnel mode)
    ↓
11. Send packet
```

### Inbound Processing (Receiving):

```
1. Receive packet
   ↓
2. Extract SPI from ESP header
   ↓
3. Lookup SA in SAD
   - Match: SPI + dst IP + protocol
   - If not found → DROP packet
   ↓
4. Check sequence number (anti-replay)
   - Must be within replay window
   - Must not be duplicate
   ↓
5. Verify ICV (authentication)
   - Calculate expected ICV
   - Compare with received ICV
   - If mismatch → DROP packet
   ↓
6. Decrypt payload
   - Use encryption algorithm and key from SA
   ↓
7. Remove padding
   ↓
8. Extract original packet
   ↓
9. Update sequence number window
   ↓
10. Deliver to upper layer
```

### Anti-Replay Protection:

```
Sliding Window (size = 64):

Sequence Numbers:
... 1001 1002 1003 1004 1005 ... 1064 1065
                         ↑              ↑
                    Window Start    Current

Received packet with seq 1003:
- Within window? YES
- Already received? Check bitmap
- If new: Accept and mark as received
- If duplicate: DROP

Received packet with seq 1066:
- Greater than current? YES
- Accept and slide window forward
```

---

## Common Interview Questions

### Q1: Explain IPsec in one sentence.
**A:** IPsec is a protocol suite that provides security at the IP layer through encryption, authentication, and integrity protection of IP packets.

### Q2: What's the difference between Transport and Tunnel mode?
**A:** 
- **Transport**: Only encrypts payload, keeps original IP header, used for host-to-host
- **Tunnel**: Encrypts entire packet, adds new IP header, used for VPN gateways

### Q3: Why is ESP more common than AH?
**A:** 
- ESP provides both encryption AND authentication
- ESP works with NAT (with NAT-T)
- AH doesn't encrypt and breaks with NAT
- Most use cases need confidentiality

### Q4: What is an SPI?
**A:** Security Parameter Index - a 32-bit identifier that, combined with destination IP and protocol, uniquely identifies a Security Association.

### Q5: How does IKE work?
**A:** IKE establishes secure channels in two phases:
1. Authenticates peers and creates IKE SA
2. Negotiates IPsec parameters and creates CHILD SA
Uses Diffie-Hellman for key exchange.

### Q6: What is Perfect Forward Secrecy?
**A:** Property where compromise of long-term keys doesn't compromise past session keys. Achieved through ephemeral Diffie-Hellman key exchange.

### Q7: How does anti-replay protection work?
**A:** Uses sequence numbers and sliding window:
- Each packet has incrementing sequence number
- Receiver maintains window (e.g., 64 packets)
- Packets outside window or duplicates are dropped

### Q8: What happens when SA expires?
**A:** 
- Soft lifetime: Start negotiating new SA (before expiry)
- Hard lifetime: Delete SA, stop using
- IKE rekeying establishes new SA seamlessly

### Q9: IPsec vs TLS/SSL?
**A:**
- **IPsec**: Layer 3, transparent to apps, site-to-site VPNs
- **TLS**: Layer 4/5, app-aware, HTTPS, requires app support

### Q10: What is NAT-T?
**A:** NAT Traversal - encapsulates ESP in UDP (port 4500) to pass through NAT devices, since NAT modifies IP headers which breaks ESP.

---

## Real-World Scenarios

### Scenario 1: Site-to-Site VPN

**Setup:**
```
Office A                           Office B
10.0.0.0/24                        192.168.0.0/24
    ↓                                    ↓
Gateway A                           Gateway B
(Public IP: 1.2.3.4)               (Public IP: 5.6.7.8)
    └────────── IPsec Tunnel ───────────┘
```

**Configuration:**
- Mode: Tunnel
- Protocol: ESP
- Encryption: AES-256-GCM
- Authentication: Certificates
- PFS: Yes (DH Group 19)

### Scenario 2: Remote Access VPN

**Setup:**
```
Remote Employee Laptop
    ↓
[IPsec Client]
    ↓
  Internet
    ↓
[Corporate VPN Gateway]
    ↓
Corporate Network
```

**Configuration:**
- Mode: Tunnel
- IKEv2 with EAP authentication
- Split tunneling (optional)
- Dynamic IP assignment

### Scenario 3: Cloud Connectivity

**AWS VPN Connection:**
```
On-Premises Network
    ↓
Customer Gateway
    ↓
IPsec Tunnel (HA with 2 tunnels)
    ↓
AWS Virtual Private Gateway
    ↓
VPC (Virtual Private Cloud)
```

**Characteristics:**
- BGP for dynamic routing
- Redundant tunnels for HA
- Pre-configured by cloud provider

---

## Troubleshooting Tips

### Common Issues:

1. **Phase 1 (IKE SA) Failure**
   - Check: Authentication (PSK, certificates)
   - Check: Crypto proposal mismatch
   - Check: Firewall (UDP 500, 4500)

2. **Phase 2 (IPsec SA) Failure**
   - Check: Traffic selectors mismatch
   - Check: Proxy IDs
   - Check: PFS group mismatch

3. **Packets Not Encrypted**
   - Check SPD rules
   - Check: Routing (traffic must go through IPsec interface)

4. **MTU Issues**
   - IPsec adds overhead (~50-70 bytes)
   - May need to reduce MTU
   - Enable PMTUD or set MSS clamping

5. **NAT Traversal Issues**
   - Check UDP 4500 is open
   - Check NAT-T is enabled on both sides
   - Keepalives to maintain NAT mapping

---

## Summary

**IPsec is powerful but complex:**
- ✅ Industry standard for VPNs
- ✅ Strong security
- ✅ Transparent to applications
- ⚠️ Configuration complexity
- ⚠️ Troubleshooting can be difficult

**For your interview:**
- Understand ESP vs AH
- Know the difference between transport and tunnel mode
- Explain IKE process clearly
- Understand SA concept
- Be ready to discuss real-world scenarios

**Practice explaining:** "How does a packet go through IPsec from application to wire?"

Good luck! 🚀
