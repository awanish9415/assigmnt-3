# WireGuard Complete Guide - Interview Preparation

## Table of Contents
1. [What is WireGuard?](#what-is-wireguard)
2. [Why WireGuard?](#why-wireguard)
3. [Architecture & Design](#architecture--design)
4. [Cryptography](#cryptography)
5. [Protocol Details](#protocol-details)
6. [Configuration](#configuration)
7. [Comparison with IPsec](#comparison-with-ipsec)
8. [Implementation Details](#implementation-details)
9. [Common Interview Questions](#common-interview-questions)
10. [Real-World Usage](#real-world-usage)

---

## What is WireGuard?

**WireGuard** is a modern, extremely simple, fast, and secure VPN protocol.

### Key Characteristics:
- **Simple**: ~4,000 lines of code (vs ~400,000 for IPsec)
- **Fast**: Faster than IPsec and OpenVPN
- **Secure**: State-of-the-art cryptography
- **Modern**: Built for today's networks
- **Cross-platform**: Linux, Windows, macOS, iOS, Android, FreeBSD

### Created By:
- Jason A. Donenfeld (zx2c4)
- First released: 2016
- Merged into Linux kernel: 5.6 (March 2020)

### Philosophy:
> "WireGuard aims to be as easy to configure and deploy as SSH"

---

## Why WireGuard?

### Problems with Existing VPNs:

#### IPsec Issues:
- ❌ Extremely complex (~400,000 lines of code)
- ❌ Difficult to configure correctly
- ❌ Many cipher suites (security nightmare)
- ❌ Slow handshake (multiple round trips)
- ❌ Large attack surface

#### OpenVPN Issues:
- ❌ Userspace (slower than kernel)
- ❌ Complex codebase (~100,000 lines)
- ❌ OpenSSL dependency issues
- ❌ Configuration complexity
- ❌ Protocol overhead

### WireGuard Solutions:

✅ **Simplicity**: Minimal code = fewer bugs  
✅ **Performance**: Kernel-space, optimized crypto  
✅ **Security**: Modern cryptography only  
✅ **Auditability**: Small codebase, easy to review  
✅ **Roaming**: Seamless IP/port changes  
✅ **Stealth**: No response to unauthorized packets  

---

## Architecture & Design

### Core Principles:

#### 1. Cryptokey Routing
- No concept of "users" or "authentication"
- Public keys are identities
- Routes are bound to public keys
- Simple and elegant

```
Peer Configuration:
PublicKey = ABC123...
AllowedIPs = 10.0.0.2/32, 192.168.1.0/24

Meaning:
- Packets FROM 10.0.0.2 must be authenticated with ABC123...
- Packets TO 10.0.0.2 or 192.168.1.0/24 are sent to peer ABC123...
```

#### 2. Silent Protocol
- Doesn't respond to unauthenticated packets
- No version information leaked
- Invisible to port scanners
- Built-in DoS protection

#### 3. No Cipher Agility
- One secure suite, period
- No negotiation needed
- Reduces complexity
- Prevents downgrade attacks

### Protocol State Machine:

```
┌─────────────────────────────────────────┐
│           IDLE STATE                     │
│  (No active session)                     │
└──────────────┬──────────────────────────┘
               │
               │ Handshake Initiation
               ↓
┌─────────────────────────────────────────┐
│      HANDSHAKE IN PROGRESS               │
│  (Waiting for response)                  │
└──────────────┬──────────────────────────┘
               │
               │ Handshake Response
               ↓
┌─────────────────────────────────────────┐
│        ACTIVE SESSION                    │
│  (Can send/receive data)                 │
│  - Rekey every 2 minutes (if traffic)    │
│  - Timeout after 3 minutes (no traffic)  │
└─────────────────────────────────────────┘
```

---

## Cryptography

### Fixed Cryptographic Suite:

WireGuard uses **ONLY** these algorithms (no negotiation):

```
┌──────────────────────────────────────────┐
│      WireGuard Cryptographic Suite        │
├──────────────────────────────────────────┤
│ Key Exchange:      Curve25519 (ECDH)     │
│ Cipher:            ChaCha20              │
│ Authenticator:     Poly1305              │
│ Hash:              BLAKE2s               │
│ Hash (keyed):      BLAKE2s (keyed)       │
│ KDF:               HKDF (BLAKE2s based)  │
└──────────────────────────────────────────┘
```

### Why These Choices?

#### Curve25519
- **Type**: Elliptic Curve Diffie-Hellman
- **Security**: ~128-bit security level
- **Speed**: Extremely fast
- **Safety**: Designed to avoid implementation mistakes
- **Used by**: Signal, SSH, TLS 1.3

#### ChaCha20
- **Type**: Stream cipher
- **Key Size**: 256-bit
- **Speed**: Faster than AES on software
- **Security**: Strong (used in TLS)
- **Advantage**: Constant-time (resistant to timing attacks)

#### Poly1305
- **Type**: Message Authentication Code
- **Security**: 128-bit
- **Speed**: Very fast
- **Used with**: ChaCha20 (AEAD construction)

#### BLAKE2s
- **Type**: Cryptographic hash
- **Security**: Strong (better than SHA-2)
- **Speed**: Faster than MD5 but secure
- **Output**: 256-bit

### Noise Protocol Framework:

WireGuard is based on **Noise_IK** pattern:

```
Noise_IK:
- I = Initiator knows responder's static public key
- K = Responder knows initiator's static public key

Properties:
✓ 1-RTT (one round trip) handshake
✓ Perfect Forward Secrecy
✓ Identity hiding of initiator
✓ Mutual authentication
```

---

## Protocol Details

### Packet Types:

```
Type 1: Handshake Initiation (148 bytes)
Type 2: Handshake Response (92 bytes)
Type 3: Cookie Reply (64 bytes)
Type 4: Transport Data (variable)
```

### 1. Handshake Initiation (Type 1)

```
┌────────────────────────────────────┐
│ Type (1 byte): 0x01                │
│ Reserved (3 bytes): 0x000000       │
│ Sender Index (4 bytes)             │
│ Ephemeral Public Key (32 bytes)   │
│ Encrypted Static Key (48 bytes)   │ ← Encrypted with responder's pubkey
│ Encrypted Timestamp (28 bytes)    │ ← For replay protection
│ MAC1 (16 bytes)                    │ ← Authenticates message
│ MAC2 (16 bytes)                    │ ← Optional (cookie for DoS protection)
├────────────────────────────────────┤
│ Total: 148 bytes                   │
└────────────────────────────────────┘
```

**Cryptographic Operations:**
```
1. Generate ephemeral keypair (eph_priv, eph_pub)
2. Compute shared secret: ECDH(eph_priv, responder_static_pub)
3. Derive encryption key using HKDF
4. Encrypt our static public key: AEAD(our_static_pub)
5. Encrypt timestamp: AEAD(current_time)
6. Compute MAC1: Hash(message || responder_pub)
7. Compute MAC2: Hash(message || cookie) [optional]
```

### 2. Handshake Response (Type 2)

```
┌────────────────────────────────────┐
│ Type (1 byte): 0x02                │
│ Reserved (3 bytes): 0x000000       │
│ Sender Index (4 bytes)             │
│ Receiver Index (4 bytes)           │
│ Ephemeral Public Key (32 bytes)   │
│ Encrypted Nothing (16 bytes)      │ ← Just auth tag (proves key derivation)
│ MAC1 (16 bytes)                    │
│ MAC2 (16 bytes)                    │
├────────────────────────────────────┤
│ Total: 92 bytes                    │
└────────────────────────────────────┘
```

**Session Keys Derived:**
```
After successful handshake, both sides derive:
- Transport Send Key
- Transport Receive Key
- Counter (nonce) = 0

Perfect Forward Secrecy achieved through ephemeral keys
```

### 3. Transport Data (Type 4)

```
┌────────────────────────────────────┐
│ Type (1 byte): 0x04                │
│ Reserved (3 bytes): 0x000000       │
│ Receiver Index (4 bytes)           │
│ Counter (8 bytes)                  │ ← Packet counter (nonce)
│ Encrypted Payload (variable)      │
│ Auth Tag (16 bytes)                │ ← Poly1305
├────────────────────────────────────┤
│ Overhead: 32 bytes                 │
└────────────────────────────────────┘
```

**Encryption:**
```
Plaintext: IP packet
Key: Session transport key
Nonce: Counter (incremented for each packet)
Algorithm: ChaCha20-Poly1305 AEAD
Ciphertext: Encrypted payload + 16 byte auth tag
```

### Handshake Flow:

```
Alice                                    Bob
-----                                    ---

Generate ephemeral keypair
Mix in Bob's static public key
Encrypt Alice's static public key
Encrypt timestamp
────── Handshake Init (148 bytes) ────►

                            Verify MAC1
                            Decrypt Alice's static key
                            Verify timestamp (anti-replay)
                            Generate ephemeral keypair
                            Derive session keys
                            Encrypt empty payload
◄───── Handshake Response (92 bytes) ──

Verify MAC1
Decrypt response
Derive session keys
────────── Data Packets ───────────────►
◄───────── Data Packets ───────────────
```

**Total:** 240 bytes, 1-RTT

---

## Configuration

### Basic Configuration Example:

#### Server Configuration (`wg0.conf`):
```ini
[Interface]
PrivateKey = SERVER_PRIVATE_KEY
Address = 10.0.0.1/24
ListenPort = 51820
PostUp = iptables -A FORWARD -i wg0 -j ACCEPT
PostDown = iptables -D FORWARD -i wg0 -j ACCEPT

[Peer]
# Client 1
PublicKey = CLIENT1_PUBLIC_KEY
AllowedIPs = 10.0.0.2/32

[Peer]
# Client 2
PublicKey = CLIENT2_PUBLIC_KEY
AllowedIPs = 10.0.0.3/32
```

#### Client Configuration:
```ini
[Interface]
PrivateKey = CLIENT_PRIVATE_KEY
Address = 10.0.0.2/32
DNS = 10.0.0.1

[Peer]
PublicKey = SERVER_PUBLIC_KEY
Endpoint = server.example.com:51820
AllowedIPs = 0.0.0.0/0  # Route all traffic through VPN
PersistentKeepalive = 25
```

### Configuration Parameters:

#### Interface Section:
```
PrivateKey      = Your private key (base64)
Address         = VPN IP address(es)
ListenPort      = UDP port (default: 51820)
DNS             = DNS servers (optional)
MTU             = Maximum Transmission Unit (optional)
Table           = Routing table number (default: auto)
PreUp/PostUp    = Commands to run when interface comes up
PreDown/PostDown = Commands to run when interface goes down
```

#### Peer Section:
```
PublicKey           = Peer's public key (base64)
Endpoint            = Peer's IP:port (for initiating connection)
AllowedIPs          = IPs that this peer can send/receive
PersistentKeepalive = Seconds between keepalive packets (for NAT)
PresharedKey        = Additional symmetric key (optional, post-quantum)
```

### Key Generation:

```bash
# Generate private key
wg genkey > privatekey

# Derive public key from private key
wg pubkey < privatekey > publickey

# Generate preshared key (optional)
wg genpsk > presharedkey
```

### AllowedIPs Explained:

**AllowedIPs serves TWO purposes:**

1. **Outbound**: Which destination IPs to route through this peer
2. **Inbound**: Which source IPs are allowed from this peer

```
Examples:

AllowedIPs = 10.0.0.2/32
→ Only route 10.0.0.2 through this peer
→ Only accept packets from 10.0.0.2

AllowedIPs = 0.0.0.0/0, ::/0
→ Route ALL traffic through this peer (full tunnel)
→ Accept packets claiming any source IP (risky!)

AllowedIPs = 10.0.0.0/24, 192.168.1.0/24
→ Split tunnel: only route these subnets
→ Accept packets from these subnets
```

### Roaming:

WireGuard automatically handles endpoint changes:

```
Initial connection:
Client (IP: 1.2.3.4, Port: 12345) → Server

Client moves to new network:
Client (IP: 5.6.7.8, Port: 54321) → Server

WireGuard automatically:
✓ Updates endpoint to 5.6.7.8:54321
✓ No interruption to connection
✓ No re-handshake needed
✓ Seamless roaming!
```

---

## Comparison with IPsec

### Code Complexity:

```
┌────────────────────────────────────┐
│        Lines of Code                │
├────────────────────────────────────┤
│ WireGuard:        ~4,000           │
│ OpenVPN:        ~100,000           │
│ IPsec (strongSwan): ~400,000      │
└────────────────────────────────────┘

Fewer lines = Fewer bugs = More secure
```

### Handshake Comparison:

```
WireGuard (1-RTT):
───────────────────────────
Client → Server: Init (148B)
Server → Client: Response (92B)
[Session established: 240 bytes, 1 round trip]

IPsec IKEv2 (2-RTT):
───────────────────────────
Client → Server: IKE_SA_INIT
Server → Client: IKE_SA_INIT
Client → Server: IKE_AUTH
Server → Client: IKE_AUTH
[Session established: 4 messages, 2 round trips, >1KB]
```

### Performance Benchmarks:

```
Throughput (Gbps) on modern hardware:
────────────────────────────────────
WireGuard:    9.8 Gbps
IPsec:        5.5 Gbps
OpenVPN:      2.1 Gbps

Latency (additional ms):
────────────────────────────────────
WireGuard:    +0.2 ms
IPsec:        +0.4 ms
OpenVPN:      +1.2 ms
```

### Feature Comparison:

| Feature | WireGuard | IPsec | OpenVPN |
|---------|-----------|-------|---------|
| **Complexity** | ⭐⭐⭐⭐⭐ Very Simple | ⭐ Complex | ⭐⭐ Moderate |
| **Performance** | ⭐⭐⭐⭐⭐ Fastest | ⭐⭐⭐⭐ Fast | ⭐⭐ Slow |
| **Security** | ⭐⭐⭐⭐⭐ Modern | ⭐⭐⭐⭐ Strong | ⭐⭐⭐⭐ Strong |
| **Roaming** | ⭐⭐⭐⭐⭐ Seamless | ⭐⭐ Poor | ⭐⭐⭐ Good |
| **Configuration** | ⭐⭐⭐⭐⭐ Easy | ⭐ Difficult | ⭐⭐⭐ Moderate |
| **Stealth** | ⭐⭐⭐⭐⭐ Silent | ⭐⭐ Responds | ⭐⭐ Responds |
| **NAT Traversal** | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐ NAT-T needed | ⭐⭐⭐⭐ Good |
| **Maturity** | ⭐⭐⭐ Young | ⭐⭐⭐⭐⭐ Very Mature | ⭐⭐⭐⭐ Mature |
| **Audit/Compliance** | ⭐⭐⭐ Growing | ⭐⭐⭐⭐⭐ Certified | ⭐⭐⭐⭐ Good |

### When to Use Each:

#### Use WireGuard When:
✅ Building new VPN infrastructure  
✅ Need maximum performance  
✅ Want simple configuration  
✅ Mobile clients (roaming)  
✅ Container networking  
✅ Modern cloud infrastructure  
✅ Personal VPN  

#### Use IPsec When:
✅ Enterprise with existing IPsec  
✅ Compliance requires IPsec (FIPS)  
✅ Hardware acceleration available  
✅ Need specific cipher suites  
✅ Regulatory requirements  
✅ Long-established infrastructure  

#### Use OpenVPN When:
✅ Need TCP mode (very restrictive networks)  
✅ Complex routing requirements  
✅ Need userspace VPN  
✅ Platform doesn't support WireGuard  
✅ Existing OpenVPN deployment  

---

## Implementation Details

### Linux Kernel Module:

```c
// Simplified structure

struct wireguard_device {
    struct net_device *dev;
    struct noise_static_identity static_identity;
    struct list_head peer_list;
    // ...
};

struct wireguard_peer {
    struct wireguard_device *device;
    struct noise_keypairs keypairs;
    struct endpoint endpoint;
    struct allowedips_node allowedips;
    u64 persistent_keepalive_interval;
    // ...
};
```

### Packet Flow (Transmit):

```
1. Application sends packet
   ↓
2. Kernel routing determines it goes to wg0
   ↓
3. WireGuard checks cryptokey routing (AllowedIPs)
   ↓
4. If no valid session, initiate handshake
   ↓
5. Increment packet counter
   ↓
6. Encrypt with ChaCha20-Poly1305
   ↓
7. Wrap in WireGuard header (Type 4)
   ↓
8. Send UDP packet to peer's endpoint
```

### Packet Flow (Receive):

```
1. UDP packet arrives on port 51820
   ↓
2. Extract receiver index
   ↓
3. Lookup session by index
   ↓
4. Verify counter (anti-replay)
   ↓
5. Decrypt and verify auth tag
   ↓
6. Check source IP against AllowedIPs
   ↓
7. Update endpoint if changed (roaming)
   ↓
8. Deliver decrypted packet to kernel
```

### Timer System:

```
Timers in WireGuard:

1. Handshake Timeout: 5 seconds
   - If no response, retry handshake

2. Rekey Timeout: 2 minutes
   - If traffic exists, create new session

3. Keepalive: 10-25 seconds (configurable)
   - Maintain NAT mappings

4. Session Timeout: 3 minutes of inactivity
   - Delete session, require new handshake

5. Cookie Refresh: Under load
   - DoS protection mechanism
```

### DoS Protection (Cookie Mechanism):

```
Normal Operation:
Client ──── Handshake Init ────► Server
Client ◄─── Handshake Response ── Server

Under Load:
Client ──── Handshake Init ────► Server
Client ◄─── Cookie Reply ───────── Server
                (tells client to include cookie in MAC2)
Client ──── Handshake Init + Cookie ─► Server
Client ◄─── Handshake Response ───── Server

Cookie proves client can receive at claimed IP
Similar to TCP SYN cookies
```

---

## Common Interview Questions

### Q1: What makes WireGuard faster than IPsec?
**A:** Several factors:
- Simpler codebase (less overhead)
- Modern ciphers optimized for software (ChaCha20)
- No cipher negotiation
- Efficient state machine
- Kernel-space implementation
- Better CPU cache utilization
- 1-RTT handshake vs 2-RTT

### Q2: Why no cipher negotiation?
**A:** Cipher agility is a security anti-pattern:
- Adds complexity (more code = more bugs)
- Enables downgrade attacks
- Configuration errors common
- One well-chosen suite is better
- If current suite is broken, upgrade protocol version

### Q3: How does WireGuard handle roaming?
**A:** Cryptokey routing:
- Public keys are identities, not IP addresses
- When authenticated packet arrives from new endpoint
- Automatically update endpoint
- No re-handshake needed
- Seamless for mobile devices

### Q4: What is the Noise Protocol Framework?
**A:** Cryptographic framework for building protocols:
- WireGuard uses "Noise_IK" pattern
- Provides templates for handshakes
- Formally verified properties
- Used by Signal, WhatsApp, etc.
- Guarantees: PFS, mutual auth, identity hiding

### Q5: How does anti-replay work?
**A:** Counter-based nonce:
- Each packet has incrementing 64-bit counter
- Receiver maintains bitmap of recent counters
- Packets with old/duplicate counters rejected
- Sliding window (typically 2048 packets)
- Prevents replay attacks

### Q6: What is the PersistentKeepalive for?
**A:** NAT traversal:
- Sends empty packet every N seconds
- Maintains NAT mapping
- Allows server to send to client behind NAT
- Not needed if client initiates all traffic
- Typical value: 25 seconds

### Q7: Can WireGuard replace IPsec in enterprise?
**A:** Depends on requirements:
- ✅ Technical: Yes, often better performance
- ⚠️ Compliance: May require IPsec (FIPS, etc.)
- ⚠️ Audit: Newer, less compliance paperwork
- ⚠️ Integration: May need existing IPsec ecosystem
- ✅ Future: Growing enterprise adoption

### Q8: How does WireGuard handle key rotation?
**A:** Automatic rekeying:
- New handshake every 2 minutes (if traffic)
- Smooth transition between old and new keys
- Perfect Forward Secrecy maintained
- Transparent to applications
- No manual intervention

### Q9: What about post-quantum cryptography?
**A:** Preshared key option:
- Optional symmetric key per peer
- Added to key derivation
- Provides post-quantum protection
- "Belt and suspenders" approach
- If quantum breaks ECC, PSK still protects

### Q10: WireGuard disadvantages?
**A:** 
- ❌ Young ecosystem (improving rapidly)
- ❌ No dynamic IP assignment (use userspace tools)
- ❌ No built-in user auth (need external tool)
- ❌ Limited compliance certifications (yet)
- ❌ Fixed crypto (good and bad)
- ❌ IP addresses must be pre-configured

---

## Real-World Usage

### 1. Personal VPN

```
Laptop/Phone ──────► WireGuard Server ──────► Internet
(Roaming)              (Fixed IP)
```

**Use case:** Privacy, security on public WiFi

### 2. Site-to-Site VPN

```
Office A                         Office B
10.0.0.0/24                      192.168.0.0/24
    ↓                                  ↓
WireGuard Gateway ←──────→ WireGuard Gateway
```

**Use case:** Connect office networks

### 3. Mesh Network (Tailscale)

```
     Device A
       / \
      /   \
Device B - Device C
     |
  Device D
```

**Use case:** All devices can reach each other directly

### 4. Kubernetes CNI

```
Pod-to-Pod communication across nodes
Encrypted overlay network
Better performance than VXLAN
```

### 5. Cloud Connectivity

```
On-Premises ←─ WireGuard ─→ Cloud VPC
              Tunnel
```

**Use case:** Hybrid cloud, secure cloud access

### Popular Services Using WireGuard:

- **Tailscale**: Mesh VPN built on WireGuard
- **Mullvad VPN**: Privacy-focused VPN
- **IVPN**: Privacy VPN
- **ProtonVPN**: Secure VPN
- **Cloudflare WARP**: Mobile security
- **Firezone**: Self-hosted VPN
- **NetBird**: Zero-trust network

---

## Tools and Commands

### Basic Commands:

```bash
# Show interface status
wg show

# Show specific interface
wg show wg0

# Add peer
wg set wg0 peer PUBLIC_KEY allowed-ips 10.0.0.2/32

# Remove peer
wg set wg0 peer PUBLIC_KEY remove

# Change endpoint
wg set wg0 peer PUBLIC_KEY endpoint 1.2.3.4:51820

# Enable interface
wg-quick up wg0

# Disable interface
wg-quick down wg0

# Check connection
ping -I wg0 10.0.0.1
```

### Debugging:

```bash
# Check interface
ip addr show wg0

# Check routes
ip route show table all | grep wg0

# Monitor traffic
tcpdump -i wg0

# Check UDP port
ss -ulnp | grep 51820

# Kernel logs
dmesg | grep wireguard

# Debug mode (if compiled with)
echo module wireguard +p > /sys/kernel/debug/dynamic_debug/control
```

---

## Summary

### Key Takeaways:

1. **Simplicity is Security**
   - 4,000 lines vs 400,000
   - Easy to audit
   - Fewer bugs

2. **Modern Cryptography**
   - ChaCha20-Poly1305
   - Curve25519
   - No legacy algorithms

3. **Performance**
   - Faster than alternatives
   - Lower latency
   - Efficient implementation

4. **User Experience**
   - Easy configuration
   - Seamless roaming
   - Works great on mobile

5. **Future**
   - Growing adoption
   - Built into Linux kernel
   - Industry momentum

### For Your Interview:

**Be ready to explain:**
- Why WireGuard is faster/simpler than IPsec
- Cryptokey routing concept
- 1-RTT handshake process
- Noise Protocol Framework basics
- Roaming mechanism
- When to use WireGuard vs IPsec

**Practice drawing:**
- Handshake message flow
- Packet structure (Type 4)
- Cryptokey routing diagram

**Know the numbers:**
- ~4,000 lines of code
- 1-RTT handshake
- 240 bytes total handshake
- 32 bytes data packet overhead

Good luck with your interview! 🚀
