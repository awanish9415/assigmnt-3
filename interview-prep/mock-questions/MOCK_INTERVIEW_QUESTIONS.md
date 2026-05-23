# Protocol Stack Engineer - Mock Interview Questions

## Table of Contents
1. [C/C++ Programming Questions](#cc-programming-questions)
2. [Networking Fundamentals](#networking-fundamentals)
3. [IPsec Specific Questions](#ipsec-specific-questions)
4. [WireGuard Specific Questions](#wireguard-specific-questions)
5. [Protocol Stack Implementation](#protocol-stack-implementation)
6. [System Design Questions](#system-design-questions)
7. [Debugging & Troubleshooting](#debugging--troubleshooting)
8. [Behavioral Questions](#behavioral-questions)

---

## C/C++ Programming Questions

### Q1: What is the difference between malloc and calloc?
**Answer:**
- **malloc**: Allocates specified bytes, memory contains garbage values
- **calloc**: Allocates memory for array, initializes all bytes to zero
```c
int *arr1 = (int*)malloc(10 * sizeof(int));  // Uninitialized
int *arr2 = (int*)calloc(10, sizeof(int));   // Initialized to 0
```

### Q2: Explain pointers to pointers with an example.
**Answer:**
Pointer to pointer stores address of another pointer.
```c
int value = 42;
int *ptr = &value;      // Pointer to int
int **pptr = &ptr;      // Pointer to pointer

printf("%d\n", **pptr); // Output: 42
```
**Use cases:** Dynamic 2D arrays, function arguments to modify pointers

### Q3: What is memory leak and how do you prevent it?
**Answer:**
Memory leak occurs when allocated memory is not freed.
```c
void leak_example() {
    char *data = (char*)malloc(100);
    // ... use data ...
    // FORGOT to call free(data) - LEAK!
}

void correct_example() {
    char *data = (char*)malloc(100);
    // ... use data ...
    free(data);  // Properly freed
}
```
**Prevention:**
- Always pair malloc/calloc with free
- Use valgrind to detect leaks
- RAII in C++ (smart pointers)
- Static analysis tools



### Q4: Explain different types of memory segments in C program.
**Answer:**
```
┌─────────────────┐ High Address
│     Stack       │ ← Local variables, function calls
├─────────────────┤
│       ↓         │
│    (grows down) │
│                 │
│       ↑         │
│    (grows up)   │
├─────────────────┤
│      Heap       │ ← malloc/calloc allocated memory
├─────────────────┤
│   BSS Segment   │ ← Uninitialized global/static variables
├─────────────────┤
│   Data Segment  │ ← Initialized global/static variables
├─────────────────┤
│   Text Segment  │ ← Program code (read-only)
└─────────────────┘ Low Address
```

### Q5: What is endianness? How do you convert between byte orders?
**Answer:**
- **Big-Endian**: MSB stored at lowest address (network byte order)
- **Little-Endian**: LSB stored at lowest address (x86 CPUs)

```c
uint32_t value = 0x12345678;

// Big-endian:    [12] [34] [56] [78]
// Little-endian: [78] [56] [34] [12]

// Conversion functions:
uint16_t net = htons(host);  // Host to Network Short
uint32_t net = htonl(host);  // Host to Network Long
uint16_t host = ntohs(net);  // Network to Host Short
uint32_t host = ntohl(net);  // Network to Host Long
```



### Q6: Write a function to reverse a linked list.
**Answer:**
```c
struct Node {
    int data;
    struct Node *next;
};

struct Node* reverse_list(struct Node *head) {
    struct Node *prev = NULL;
    struct Node *current = head;
    struct Node *next = NULL;
    
    while (current != NULL) {
        next = current->next;    // Save next
        current->next = prev;    // Reverse link
        prev = current;          // Move prev forward
        current = next;          // Move current forward
    }
    
    return prev;  // New head
}
```
**Time Complexity:** O(n)  
**Space Complexity:** O(1)

### Q7: What is the difference between struct and union?
**Answer:**
- **struct**: All members have separate memory, size = sum of all members
- **union**: All members share same memory, size = largest member

```c
struct MyStruct {
    int a;    // 4 bytes
    char b;   // 1 byte
    float c;  // 4 bytes
};  // Size: 12 bytes (with padding)

union MyUnion {
    int a;    // 4 bytes
    char b;   // 1 byte
    float c;  // 4 bytes
};  // Size: 4 bytes (largest member)
```

**Use case for union:** IP address representation
```c
union ip_addr {
    uint32_t addr;       // Single 32-bit value
    uint8_t bytes[4];    // Four octets
};
```



### Q8: Explain volatile keyword in C.
**Answer:**
`volatile` tells compiler the variable can change unexpectedly (outside program control).

**Use cases:**
1. Hardware registers (memory-mapped I/O)
2. Variables modified by ISR (Interrupt Service Routine)
3. Multi-threaded shared variables

```c
volatile uint32_t *reg = (uint32_t*)0x40000000;  // Hardware register
*reg = 0x01;  // Compiler won't optimize this away

// Without volatile, compiler might optimize:
int x = *reg;
int y = *reg;  // Compiler thinks this is redundant
// But register value might have changed!

// With volatile:
volatile int x = *reg;
volatile int y = *reg;  // Compiler reads twice
```

### Q9: What is the difference between deep copy and shallow copy?
**Answer:**
```c
struct Packet {
    int size;
    char *data;
};

// Shallow Copy - copies pointer, not data
void shallow_copy(struct Packet *dest, struct Packet *src) {
    dest->size = src->size;
    dest->data = src->data;  // Both point to SAME memory
}

// Deep Copy - copies data itself
void deep_copy(struct Packet *dest, struct Packet *src) {
    dest->size = src->size;
    dest->data = (char*)malloc(src->size);
    memcpy(dest->data, src->data, src->size);  // NEW memory
}
```

### Q10: How do you detect a loop in a linked list?
**Answer:**
**Floyd's Cycle Detection** (Tortoise and Hare):
```c
int has_loop(struct Node *head) {
    struct Node *slow = head;
    struct Node *fast = head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;         // Move 1 step
        fast = fast->next->next;   // Move 2 steps
        
        if (slow == fast) {
            return 1;  // Loop detected
        }
    }
    
    return 0;  // No loop
}
```
**Time:** O(n), **Space:** O(1)



---

## Networking Fundamentals

### Q11: Explain TCP 3-way handshake in detail.
**Answer:**
```
Client                          Server
------                          ------
[CLOSED]                        [LISTEN]
   │                               │
   │  SYN (seq=100)                │
   ├──────────────────────────────►│
   │      [SYN_SENT]                │ [SYN_RECEIVED]
   │                               │
   │  SYN-ACK (seq=300, ack=101)   │
   │◄──────────────────────────────┤
   │                               │
   │  ACK (seq=101, ack=301)       │
   ├──────────────────────────────►│
   │                               │
[ESTABLISHED]                [ESTABLISHED]
```

**Purpose:**
1. Synchronize sequence numbers
2. Exchange initial parameters (MSS, window size)
3. Establish reliable connection

**Why 3-way?** 2-way isn't enough:
- Prevents old duplicate SYN from opening connection
- Both sides confirm readiness

### Q12: What is the difference between flow control and congestion control?
**Answer:**

**Flow Control:**
- Prevents sender from overwhelming **receiver**
- Receiver advertises window size
- Controlled by receiver's buffer capacity
- Per-connection

**Congestion Control:**
- Prevents sender from overwhelming **network**
- Sender adjusts based on packet loss
- Controlled by network capacity
- Affects all connections

```
Flow Control:
Sender → [Network OK] → Receiver (buffer full)
                        ↓
                   "Slow down!"

Congestion Control:
Sender → [Network congested, packets dropping]
         ↓
    "Slow down!"
```



### Q13: How does ARP work? What is ARP poisoning?
**Answer:**

**ARP (Address Resolution Protocol):**
Maps IP address to MAC address on local network.

```
Process:
1. Host A wants to send to 192.168.1.10
2. Checks ARP cache - not found
3. Broadcasts: "Who has 192.168.1.10? Tell 192.168.1.5"
4. Host with 192.168.1.10 replies: "I'm at MAC AA:BB:CC:DD:EE:FF"
5. A caches this mapping
6. A sends packet to AA:BB:CC:DD:EE:FF
```

**ARP Poisoning (Attack):**
Attacker sends fake ARP replies to poison cache.

```
Normal:
Client → Router → Internet

ARP Poisoning:
Attacker sends:
  "Router IP is at ATTACKER_MAC"  (to client)
  "Client IP is at ATTACKER_MAC"  (to router)

Result:
Client → Attacker → Router → Internet
         (Man-in-the-Middle)
```

**Prevention:** Static ARP entries, ARP inspection (switch feature), encryption (IPsec, TLS)

### Q14: What happens when you ping 8.8.8.8?
**Answer:**

**Step-by-step:**

1. **DNS Not Needed** (IP given directly)

2. **Routing Decision:**
   - Check routing table
   - 8.8.8.8 not local, use default gateway

3. **ARP for Gateway:**
   - Need gateway MAC address
   - ARP request/reply

4. **ICMP Echo Request Created:**
   ```
   Type: 8 (Echo Request)
   Code: 0
   Identifier: Random
   Sequence: 1, 2, 3...
   Data: Optional payload
   ```

5. **IP Packet:**
   - Source: Your IP
   - Dest: 8.8.8.8
   - Protocol: 1 (ICMP)
   - TTL: 64

6. **Ethernet Frame:**
   - Dest MAC: Gateway MAC
   - Source MAC: Your MAC

7. **Transmission** through routers (TTL decrements)

8. **8.8.8.8 Responds:**
   - ICMP Echo Reply (Type 0)
   - Same identifier, sequence

9. **Output:**
   ```
   Reply from 8.8.8.8: bytes=32 time=10ms TTL=55
   ```



### Q15: Explain NAT and its types.
**Answer:**

**NAT (Network Address Translation):**
Translates private IPs to public IP(s).

**Types:**

**1. Static NAT (1-to-1):**
```
Private: 192.168.1.10 ←→ Public: 1.2.3.4
Private: 192.168.1.11 ←→ Public: 1.2.3.5
```
One private IP mapped to one public IP.

**2. Dynamic NAT (Pool):**
```
Private: 192.168.1.0/24 ←→ Public Pool: 1.2.3.4 - 1.2.3.10
```
Multiple private IPs share a pool of public IPs.

**3. PAT (Port Address Translation) / NAT Overload:**
```
192.168.1.10:5000 ←→ 1.2.3.4:12345
192.168.1.11:5000 ←→ 1.2.3.4:12346
192.168.1.12:6000 ←→ 1.2.3.4:12347
```
Multiple private IPs share ONE public IP (using different ports).

**NAT Table Example:**
```
Inside Local    Inside Global    Outside Global
192.168.1.10:5000 → 1.2.3.4:12345 → 8.8.8.8:80
```

**Advantages:**
- Conserves IPv4 addresses
- Security (hides internal topology)

**Disadvantages:**
- Breaks end-to-end connectivity
- Issues with some protocols (FTP, SIP)
- Complicates peer-to-peer
- Problems with IPsec

### Q16: What is MTU and MSS? Why are they important?
**Answer:**

**MTU (Maximum Transmission Unit):**
Maximum frame size at data link layer.
- Ethernet: 1500 bytes
- WiFi: 1500 bytes
- PPPoE: 1492 bytes

**MSS (Maximum Segment Size):**
Maximum TCP payload size.
- MSS = MTU - IP header - TCP header
- Typical: 1500 - 20 - 20 = 1460 bytes

```
┌─────────────────────────────────────┐
│       Ethernet Frame (1518)         │
├──────┬──────────────────────────────┤
│Header│    Payload (MTU = 1500)      │
└──────┴────┬─────────────────────────┘
            │
       ┌────▼──────────────────────────┐
       │      IP Packet                │
       ├──────┬────────────────────────┤
       │IP Hdr│  TCP Segment           │
       └──────┴────┬───────────────────┘
                   │
              ┌────▼────────────────────┐
              │    TCP Segment          │
              ├──────┬──────────────────┤
              │TCP   │  Data (MSS=1460) │
              │Header│                  │
              └──────┴──────────────────┘
```

**Importance:**
- Packet > MTU → Fragmentation (slow, can be blocked)
- Path MTU Discovery finds smallest MTU in path
- TCP MSS negotiated in handshake (SYN packets)



---

## IPsec Specific Questions

### Q17: Explain IPsec ESP vs AH. Which would you use and why?
**Answer:**

**ESP (Encapsulating Security Payload):**
- ✅ Encryption (confidentiality)
- ✅ Authentication & integrity
- ✅ Works with NAT (via NAT-T)
- ✅ Protocol: 50
- Most commonly used

**AH (Authentication Header):**
- ❌ No encryption
- ✅ Authentication & integrity
- ❌ Doesn't work with NAT
- ✅ Protocol: 51
- Authenticates IP header too
- Rarely used

**Recommendation:** Always use ESP
- Modern deployments need encryption
- NAT is everywhere
- ESP can provide authentication too
- AH offers no real advantage

### Q18: What is IKE and explain IKEv2 handshake process.
**Answer:**

**IKE (Internet Key Exchange):**
Protocol for:
- Peer authentication
- Key exchange (Diffie-Hellman)
- Security parameter negotiation
- Creating Security Associations

**IKEv2 Handshake (4 messages, 2 RTT):**

```
Initiator                        Responder
---------                        ---------

IKE_SA_INIT Request ────────────────────►
  - Crypto proposals (AES, DH group, etc.)
  - Diffie-Hellman public key
  - Nonce (random number)

                    ◄──────────────────── IKE_SA_INIT Response
                                          - Selected crypto
                                          - DH public key
                                          - Nonce

[Both derive keys from DH exchange]
[IKE SA established - secure channel]

IKE_AUTH Request ───────────────────────►
  (Encrypted with IKE SA keys)
  - Identity (certificate or PSK)
  - Authentication data
  - CHILD_SA proposals (for IPsec ESP)
  - Traffic selectors

                    ◄──────────────────── IKE_AUTH Response
                                          (Encrypted)
                                          - Identity
                                          - Authentication
                                          - CHILD_SA
                                          - Traffic selectors

[CHILD_SA established - ready for ESP traffic]
```

**Result:**
- IKE SA: Protects IKE messages
- CHILD SA: Actual IPsec SA for data traffic



### Q19: How does IPsec anti-replay protection work?
**Answer:**

**Mechanism:**
Uses sequence numbers and sliding window.

**Process:**

1. **Sender:**
   - Each packet gets unique, incrementing sequence number
   - Starts at 1, wraps at 2^32 - 1
   - Included in ESP header

2. **Receiver:**
   - Maintains sliding window (typically 64 packets)
   - Maintains bitmap of received packets in window
   
```
Sliding Window (size=64):
┌─────────────────────────────────────────┐
│ 1001 1002 1003 ... 1062 1063 1064 [1065]│
│  ✓    ✓    ✓   ...  ✓    ✓    ✓    ✓   │
└─────────────────────────────────────────┘
         ↑ Window Start      Current ↑

Packet with seq=1003 arrives:
- Within window? YES
- Already received? Check bitmap
- Duplicate? REJECT
- New? Accept and mark as received

Packet with seq=1066 arrives:
- Greater than current? YES
- Accept and slide window forward
```

**Protection Against:**
- Replay attacks (old packets re-sent)
- Out-of-order delivery (to extent of window size)

**What happens at sequence wrap?**
- At 2^32, SA must be rekeyed
- Prevents birthday attacks

### Q20: Explain IPsec tunnel mode vs transport mode with diagrams.
**Answer:**

**Transport Mode (Host-to-Host):**
```
Original Packet:
┌──────────┬──────┬─────────┐
│ IP Header│ TCP  │ Payload │
└──────────┴──────┴─────────┘

After ESP Transport:
┌──────────┬─────────┬────────────────────┬─────────┬─────┐
│ Original │   ESP   │   Encrypted       │   ESP   │ ICV │
│ IP Header│ Header  │   TCP + Payload    │ Trailer │     │
└──────────┴─────────┴────────────────────┴─────────┴─────┘
     ↑                        ↑                          ↑
  Visible              Encrypted                  Auth tag
```

**Characteristics:**
- IP header NOT encrypted
- Only payload encrypted
- Less overhead (~50 bytes)
- Used for end-to-end between hosts

**Tunnel Mode (Gateway-to-Gateway):**
```
Original Packet:
┌──────────┬──────┬─────────┐
│ IP Header│ TCP  │ Payload │
└──────────┴──────┴─────────┘

After ESP Tunnel:
┌─────────┬─────────┬──────────────────────────────────┬─────────┬─────┐
│   New   │   ESP   │      Encrypted                   │   ESP   │ ICV │
│IP Header│ Header  │  Original IP + TCP + Payload     │ Trailer │     │
└─────────┴─────────┴──────────────────────────────────┴─────────┴─────┘
Gateway IPs          Entire original packet encrypted
```

**Characteristics:**
- Entire original packet encrypted
- New IP header (gateway addresses)
- More overhead (~70 bytes)
- Hides internal topology
- Used for VPN gateways

**Example Use Case:**
```
Office A Network          Internet           Office B Network
(10.0.0.0/24)                               (192.168.0.0/24)
      │                                           │
Gateway A (1.2.3.4) ←─ IPsec Tunnel ─→ Gateway B (5.6.7.8)

Outer (Tunnel): 1.2.3.4 → 5.6.7.8
Inner (Original): 10.0.0.5 → 192.168.0.10
```



---

## WireGuard Specific Questions

### Q21: What makes WireGuard faster and simpler than IPsec?
**Answer:**

**1. Code Complexity:**
```
WireGuard:     ~4,000 lines
IPsec/IKE:   ~400,000 lines
```
Less code = fewer bugs = easier to audit

**2. Cryptography:**
WireGuard uses ONLY modern, fast algorithms:
- ChaCha20 (faster than AES in software)
- Poly1305 (fast authentication)
- Curve25519 (fast key exchange)

IPsec allows many algorithms → complexity

**3. Handshake:**
```
WireGuard: 1-RTT (2 messages, 240 bytes)
IPsec:     2-RTT (4+ messages, >1KB)
```

**4. State Machine:**
WireGuard has simpler state machine:
- Idle → Handshake → Active
- Automatic rekeying every 2 minutes

IPsec has complex state management

**5. No Negotiation:**
WireGuard: One cipher suite, no negotiation
IPsec: Must negotiate algorithms, modes, etc.

**6. Implementation:**
WireGuard runs in kernel space (Linux)
Optimized data path

**Result:**
- 2x throughput of IPsec
- Lower latency
- Less CPU usage



### Q22: Explain WireGuard's cryptokey routing concept.
**Answer:**

**Concept:**
Public keys ARE identities. Routes are bound to public keys.

**Traditional VPN (Username-based):**
```
User "alice" logs in with password
→ Assigned IP: 10.0.0.2
→ Can access resources
```

**WireGuard (Key-based):**
```
Peer with PublicKey ABC123...
→ AllowedIPs: 10.0.0.2/32, 192.168.1.0/24
```

**Meaning:**
1. **Outbound:** Packets destined to 10.0.0.2 or 192.168.1.0/24 are sent to peer ABC123
2. **Inbound:** Packets FROM 10.0.0.2 must be authenticated with key ABC123

**Example Configuration:**
```ini
[Interface]
PrivateKey = SERVER_PRIVATE_KEY
Address = 10.0.0.1/24
ListenPort = 51820

[Peer]
PublicKey = CLIENT_PUBLIC_KEY
AllowedIPs = 10.0.0.2/32
```

**Benefits:**
- No user database needed
- No authentication server
- Simple and secure
- Can't spoof source IP (cryptographically bound)

**Routing Decision:**
```
Packet to 10.0.0.2:
1. Check cryptokey routing table
2. Find peer with AllowedIPs containing 10.0.0.2
3. Encrypt with that peer's keys
4. Send to that peer's endpoint
```

### Q23: How does WireGuard handle roaming (IP/port changes)?
**Answer:**

**Problem:**
Mobile device changes network:
- IP address changes
- NAT port changes
- Connection should remain seamless

**WireGuard Solution:**

**1. Stateless Response:**
```
Server doesn't remember client endpoint.
Server responds to wherever authenticated packet came from.
```

**2. Automatic Endpoint Update:**
```
Initial:
Client (1.2.3.4:5000) → Server

Client moves to new network:
Client (5.6.7.8:6000) → Server

Server:
- Receives authenticated packet from 5.6.7.8:6000
- Verifies cryptographic authentication
- Updates endpoint to 5.6.7.8:6000
- Responds to new endpoint
```

**3. No Re-handshake Needed:**
Session keys remain valid, just endpoint updated.

**4. Keepalive:**
```
PersistentKeepalive = 25
```
Sends empty packet every 25 seconds to maintain NAT mapping.

**Real-World Example:**
```
1. User on WiFi at home (192.168.1.x)
2. Walks outside, switches to 4G (carrier NAT)
3. Connection seamless, no interruption
4. Enters coffee shop, switches to WiFi
5. Still connected, endpoint auto-updated
```

**Why This Works:**
- Authentication is based on cryptographic keys
- Not tied to IP/port
- Identity follows the keys, not the address



---

## Protocol Stack Implementation

### Q24: Explain how a packet travels from application to wire.
**Answer:**

**Complete Journey:**

**1. Application Layer:**
```c
write(sockfd, "GET / HTTP/1.1", 14);
```

**2. Socket Layer (Kernel):**
- Find socket structure
- Check connection state
- Copy data to socket send buffer

**3. Transport Layer (TCP):**
- Break data into segments (MSS)
- Add TCP header (ports, seq, ack, flags)
- Calculate TCP checksum (with pseudo-header)
- Pass to IP layer

**4. Network Layer (IP):**
- Add IP header (src/dst IP, TTL, protocol)
- Make routing decision:
  - Check routing table
  - Determine next hop
- Decrement TTL (if forwarding)
- Calculate IP checksum
- Pass to data link layer

**5. Netfilter Hooks:**
- OUTPUT chain (iptables rules)
- POSTROUTING chain (NAT)

**6. Data Link Layer:**
- ARP lookup for next hop MAC
- Add Ethernet header (src/dst MAC, type)
- Add FCS (Frame Check Sequence)
- Queue frame to device driver

**7. Device Driver:**
- Prepare DMA descriptors
- Program NIC registers
- Initiate transmission

**8. NIC (Network Interface Card):**
- Read frame from memory via DMA
- Generate preamble and SFD
- Serialize to electrical/optical signals
- Transmit on wire

**Optimizations:**
- **GSO/TSO**: Segmentation offload to NIC
- **Checksum offload**: NIC calculates checksums
- **Zero-copy**: sendfile(), DMA directly from file cache



### Q25: How does a firewall work at packet level?
**Answer:**

**Linux Netfilter/iptables:**

**Hook Points:**
```
                    ┌────────────┐
       Incoming ───►│ PREROUTING │
                    └──────┬─────┘
                           │
                    ┌──────▼──────┐
                    │   Routing   │
                    │  Decision   │
                    └──────┬──────┘
                           │
                  ┌────────┴────────┐
                  │                 │
            Local Dest          Forward
                  │                 │
             ┌────▼────┐      ┌─────▼─────┐
             │  INPUT  │      │  FORWARD  │
             └────┬────┘      └─────┬─────┘
                  │                 │
         Local Process         ┌────▼─────────┐
                  │            │ POSTROUTING  │
             ┌────▼────┐       └─────┬────────┘
             │ OUTPUT  │             │
             └────┬────┘             │
                  │                  │
                  └────────┬─────────┘
                           │
                       Send to wire
```

**Rule Processing:**

**Example Rule:**
```bash
iptables -A INPUT -p tcp --dport 22 -s 192.168.1.0/24 -j ACCEPT
```

**What firewall checks:**
1. **Interface**: Which interface packet arrived on
2. **Protocol**: TCP, UDP, ICMP, etc.
3. **Source IP**: Where packet came from
4. **Dest IP**: Where packet is going
5. **Source Port**: Application port (sender)
6. **Dest Port**: Application port (receiver)
7. **Flags**: TCP flags (SYN, ACK, etc.)
8. **State**: Connection state (NEW, ESTABLISHED, RELATED)

**Actions:**
- **ACCEPT**: Let packet through
- **DROP**: Silently discard
- **REJECT**: Discard and send ICMP error
- **LOG**: Log packet
- **SNAT/DNAT**: Network Address Translation

**Example Ruleset:**
```bash
# Default policy: DROP
iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT ACCEPT

# Allow established connections
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# Allow SSH from trusted network
iptables -A INPUT -p tcp --dport 22 -s 192.168.1.0/24 -j ACCEPT

# Allow HTTP/HTTPS
iptables -A INPUT -p tcp --dport 80 -j ACCEPT
iptables -A INPUT -p tcp --dport 443 -j ACCEPT

# Allow ping
iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
```

**Stateful Inspection:**
Tracks connection state:
```
Client initiates connection:
  SYN → (NEW) → Allow
Server responds:
  SYN-ACK → (ESTABLISHED) → Allow
Subsequent packets:
  → (ESTABLISHED) → Allow
```



### Q26: What is sk_buff in Linux kernel networking?
**Answer:**

**sk_buff (socket buffer):**
Core data structure representing a network packet in Linux kernel.

**Structure (simplified):**
```c
struct sk_buff {
    // Buffer pointers
    unsigned char *head;      // Start of allocated memory
    unsigned char *data;      // Start of actual data
    unsigned char *tail;      // End of actual data
    unsigned char *end;       // End of allocated memory
    
    // Metadata
    unsigned int len;         // Length of data
    unsigned int data_len;    // Length of fragmented data
    
    // Network device
    struct net_device *dev;   // Input/output device
    
    // Socket association
    struct sock *sk;          // Owning socket
    
    // Protocol headers
    struct tcphdr *th;        // TCP header
    struct udphdr *uh;        // UDP header
    struct iphdr *iph;        // IP header
    struct ipv6hdr *ipv6h;    // IPv6 header
    
    // Timestamps, checksums, etc.
    ktime_t tstamp;           // Timestamp
    __u16 transport_header;   // Offset to transport header
    __u16 network_header;     // Offset to network header
    __u16 mac_header;         // Offset to MAC header
};
```

**Layout:**
```
Memory:
┌──────────────────────────────────────────┐
│  head                              end   │
│   ↓                                 ↓    │
│   [headroom][data][tailroom]            │
│             ↑    ↑                       │
│           data  tail                     │
└──────────────────────────────────────────┘
```

**Operations:**

**1. Adding Header (e.g., TCP):**
```c
skb_push(skb, sizeof(struct tcphdr));
```
Moves `data` pointer back, increases `len`.

**2. Removing Header (e.g., after parsing):**
```c
skb_pull(skb, sizeof(struct tcphdr));
```
Moves `data` pointer forward, decreases `len`.

**3. Adding Trailer:**
```c
skb_put(skb, trailer_len);
```
Moves `tail` pointer forward.

**Packet Journey:**
```
Application → Socket Buffer
              ↓
         [sk_buff created]
              ↓
         TCP layer adds header ← skb_push()
              ↓
         IP layer adds header ← skb_push()
              ↓
         Ethernet adds header ← skb_push()
              ↓
         NIC transmits
```

**Why it's important:**
- Zero-copy: Headers added by adjusting pointers
- Efficient: No need to copy entire packet
- Metadata travels with packet through stack



---

## System Design Questions

### Q27: Design a high-performance packet processing system.
**Answer:**

**Requirements:**
- Process 10 Gbps traffic
- Low latency (<100 μs)
- DPI (Deep Packet Inspection)
- Stateful tracking

**Architecture:**

```
                    ┌─────────────────┐
    Packets ───────►│  NIC (10 Gbps)  │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │   RSS (Multi-Q)  │
                    │  Distribute to   │
                    │   CPU cores      │
                    └────────┬─────────┘
                             │
         ┌───────────────────┼───────────────────┐
         │                   │                   │
    ┌────▼────┐         ┌────▼────┐        ┌────▼────┐
    │ Core 0  │         │ Core 1  │        │ Core N  │
    │ Worker  │         │ Worker  │        │ Worker  │
    └────┬────┘         └────┬────┘        └────┬────┘
         │                   │                   │
         └───────────────────┼───────────────────┘
                             │
                    ┌────────▼─────────┐
                    │  Output Queue    │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │   NIC Transmit   │
                    └──────────────────┘
```

**Design Decisions:**

**1. Bypass Kernel (DPDK):**
```
Userspace packet processing:
- Poll mode (no interrupts)
- Huge pages for memory
- CPU pinning
- Lock-free queues
```

**2. Multi-core Processing:**
```
RSS (Receive Side Scaling):
- Hash on 5-tuple (src/dst IP, src/dst port, protocol)
- Distribute to different cores
- Each core processes subset of flows
- No shared state between cores
```

**3. Memory Management:**
```
Mempool:
- Pre-allocate packet buffers
- No malloc/free in fast path
- Cache-aligned structures
```

**4. Flow Table:**
```
Per-core hash table:
- Key: 5-tuple
- Value: Connection state
- Lock-free (per-core)
- Aging mechanism for cleanup
```

**5. Pipeline:**
```
Per packet:
1. Parse headers (L2/L3/L4)
2. Flow lookup (hash table)
3. DPI (pattern matching)
4. Action (forward/drop/modify)
5. Enqueue to output
```

**Performance Optimizations:**
- Cache-friendly data structures
- Batch processing (process multiple packets)
- Prefetching (hint CPU cache)
- SIMD instructions for parsing
- Hardware offloads (checksum, TSO)

**Expected Performance:**
- 10 Gbps: ~14.88 million packets/sec (64-byte packets)
- Per core: ~3-4 Mpps
- Need 4-5 cores



### Q28: How would you implement a VPN protocol from scratch?
**Answer:**

**High-Level Design:**

**1. Key Components:**
```
┌─────────────────────────────────────────┐
│          VPN Architecture               │
├─────────────────────────────────────────┤
│ Control Plane:                          │
│  - Handshake protocol                   │
│  - Key exchange                         │
│  - Peer management                      │
├─────────────────────────────────────────┤
│ Data Plane:                             │
│  - Packet encryption/decryption         │
│  - Tunneling                            │
│  - Routing                              │
└─────────────────────────────────────────┘
```

**2. Protocol Design:**

**Handshake:**
```
Client                           Server
------                           ------
ClientHello ──────────────────►
  - Protocol version
  - Supported algorithms
  - Client public key
  - Nonce

                  ◄────────────── ServerHello
                                  - Selected algorithm
                                  - Server public key
                                  - Nonce
                                  
[Derive session keys using ECDH]

ClientAuth ───────────────────►
  (Encrypted with session key)
  - Certificate/PSK
  - Proof of key possession

                  ◄────────────── ServerAuth
                                  (Encrypted)
                                  - Acknowledgment
                                  
[Session established]
```

**3. Packet Format:**
```
Data Packet:
┌──────┬──────────┬─────────┬────────────────┬─────────┐
│ Type │ Session  │ Counter │   Encrypted    │ Auth    │
│ (1B) │ ID (4B)  │  (8B)   │   Payload      │ Tag     │
└──────┴──────────┴─────────┴────────────────┴─────────┘
```

**4. Implementation Stack:**

```c
// Main structures
typedef struct {
    uint32_t session_id;
    uint8_t send_key[32];
    uint8_t recv_key[32];
    uint64_t send_counter;
    uint64_t recv_counter;
} vpn_session_t;

typedef struct {
    uint8_t type;
    uint32_t session_id;
    uint64_t counter;
    uint8_t encrypted_data[];
} vpn_packet_t;

// Core functions
int vpn_handshake(peer_t *peer);
int vpn_encrypt_packet(vpn_session_t *session, 
                       uint8_t *plaintext, int len,
                       uint8_t *ciphertext);
int vpn_decrypt_packet(vpn_session_t *session,
                       uint8_t *ciphertext, int len,
                       uint8_t *plaintext);
```

**5. Crypto Choices:**
- Key Exchange: Curve25519 (ECDH)
- Encryption: ChaCha20
- Authentication: Poly1305
- Hash: BLAKE2

**6. Threading Model:**
```
Thread 1: Control thread
  - Handle handshakes
  - Peer management
  - Timer management

Thread 2-N: Data threads
  - Encrypt/decrypt packets
  - One per CPU core
```

**7. Kernel Integration:**
```
Options:
a) TUN/TAP device (userspace)
   - Easy to implement
   - Moderate performance

b) Kernel module
   - Better performance
   - Complex to develop

c) eBPF/XDP
   - Best performance
   - Modern approach
```

**Challenges:**
- Replay protection
- Perfect Forward Secrecy
- Roaming support
- NAT traversal
- Performance optimization



---

## Debugging & Troubleshooting

### Q29: How would you debug a packet loss issue?
**Answer:**

**Systematic Approach:**

**1. Identify Layer:**
```
Application Layer?
    ↓ NO
Transport Layer (TCP retransmits)?
    ↓ NO
Network Layer (routing)?
    ↓ NO
Data Link Layer (interface errors)?
    ↓ NO
Physical Layer (cable/hardware)?
```

**2. Tools to Use:**

**a) ping (ICMP):**
```bash
ping -c 100 target_ip
# Check packet loss percentage
# 0% = good, >1% = investigate
```

**b) traceroute/mtr:**
```bash
mtr target_ip
# Shows loss at each hop
# Identifies where packets are dropping
```

**c) tcpdump/Wireshark:**
```bash
# Capture on both ends
tcpdump -i eth0 -w capture.pcap

# Look for:
- Retransmissions (TCP)
- Out-of-order packets
- Duplicate ACKs
- Window size issues
```

**d) netstat/ss:**
```bash
netstat -s
# Check for:
# - TCP retransmit segments
# - UDP errors
# - ICMP errors
```

**e) Interface statistics:**
```bash
ifconfig eth0
# or
ip -s link show eth0

# Check:
RX errors:  # Receive errors
TX errors:  # Transmit errors
dropped:    # Dropped packets
overruns:   # Buffer overruns
collisions: # Collision (hub/half-duplex)
```

**f) System logs:**
```bash
dmesg | grep -i eth0
journalctl -u networking

# Look for driver errors
```

**3. Common Causes & Solutions:**

**a) Network Congestion:**
```
Symptoms: High latency, packet loss under load
Solution: 
- QoS/Traffic shaping
- Increase bandwidth
- Optimize application
```

**b) MTU Mismatch:**
```
Symptoms: Small packets work, large fail
Solution:
ping -M do -s 1472 target  # Test path MTU
# Adjust MTU on interface
```

**c) Firewall dropping:**
```
Symptoms: Specific ports/protocols fail
Solution:
iptables -L -v -n  # Check rules
# Temporarily disable to test
```

**d) Interface errors:**
```
Symptoms: RX/TX errors in ifconfig
Solution:
- Check cables
- Check duplex settings (auto vs fixed)
- Update NIC driver
```

**4. Specific Scenarios:**

**TCP Retransmissions:**
```
Wireshark filter: tcp.analysis.retransmission

Causes:
- Packet loss
- Out-of-order delivery
- Window size too small

Check:
- RTT (Round Trip Time)
- Window scaling
- Congestion control state
```

**UDP Loss:**
```
Symptoms: No retransmission (UDP is unreliable)
Check application logs for missing data

Causes:
- Network congestion
- Receive buffer overflow
- Application not reading fast enough

Solution:
sysctl -w net.core.rmem_max=26214400  # Increase buffer
```



### Q30: VPN is not working. How do you troubleshoot?
**Answer:**

**Systematic Debugging:**

**Phase 1: Basic Connectivity**
```bash
# 1. Can you reach VPN server?
ping vpn.example.com

# 2. Is VPN port open?
telnet vpn.example.com 1194  # OpenVPN
telnet vpn.example.com 500   # IPsec
nc -u vpn.example.com 51820  # WireGuard

# 3. Check local firewall
iptables -L -v -n
# Ensure VPN ports allowed
```

**Phase 2: Control Plane (Handshake)**

**For IPsec:**
```bash
# Check IKE daemon
systemctl status strongswan
# or
systemctl status ipsec

# Logs
journalctl -u strongswan -f

# Common issues:
# - Authentication failure (PSK/certificate)
# - Proposal mismatch (algorithms)
# - Phase 1 success, Phase 2 fails (traffic selectors)

# Manual test
ipsec statusall
# Shows SA status
```

**For WireGuard:**
```bash
# Check interface
wg show

# Should show:
# - peer public key
# - endpoint
# - latest handshake time
# - transfer stats

# If no handshake:
wg show wg0 latest-handshakes
# Should be recent (<3 minutes)

# Check allowed-ips
wg show wg0 allowed-ips
```

**Phase 3: Data Plane**

```bash
# Ping through tunnel
ping -I wg0 10.0.0.1  # Tunnel IP

# If control plane OK but data fails:

# 1. Check routing
ip route show table all
# Ensure routes pointing to VPN interface

# 2. Check IP forwarding (server side)
sysctl net.ipv4.ip_forward
# Should be 1

# 3. Check NAT/masquerading (server side)
iptables -t nat -L -v -n
# MASQUERADE rule needed for internet access

# 4. Capture packets
tcpdump -i wg0 -n
# See if encrypted packets arrive
```

**Phase 4: DNS**
```bash
# Can resolve names through VPN?
nslookup google.com

# Check DNS configuration
cat /etc/resolv.conf
# Should have VPN DNS server
```

**Common Issues & Fixes:**

**1. MTU Problems:**
```bash
# VPN adds overhead
# Ethernet MTU: 1500
# After VPN: ~1420-1440

# Test
ping -M do -s 1400 target
# If works, MTU issue

# Fix
ip link set dev wg0 mtu 1420
```

**2. Split Tunnel vs Full Tunnel:**
```bash
# Check routes
ip route

# Split tunnel: Only specific routes through VPN
# Full tunnel: Default route (0.0.0.0/0) through VPN

# If wrong:
# Add route
ip route add 10.0.0.0/8 dev wg0

# Delete route
ip route del 0.0.0.0/0 dev wg0
```

**3. Firewall Blocking:**
```bash
# Server side: Allow VPN traffic
iptables -A INPUT -i wg0 -j ACCEPT
iptables -A FORWARD -i wg0 -j ACCEPT

# Allow outgoing
iptables -A FORWARD -o wg0 -j ACCEPT
```

**4. Keepalive Issues (NAT):**
```bash
# WireGuard config
PersistentKeepalive = 25

# IPsec: DPD (Dead Peer Detection)
```

**Phase 5: Performance Issues**

```bash
# Slow VPN connection

# 1. Test bandwidth
iperf3 -c vpn_server

# 2. Check CPU usage
top
# Is VPN process using 100% CPU?

# 3. Check encryption overhead
# Some CPUs have AES-NI (hardware acceleration)
grep aes /proc/cpuinfo

# 4. Check if compression enabled (usually bad idea)
```

**Debugging Checklist:**
```
☐ Physical connectivity (ping server)
☐ Port reachability (telnet/nc)
☐ Firewall rules (iptables)
☐ Authentication (logs)
☐ Handshake success (wg show / ipsec status)
☐ Routing (ip route)
☐ IP forwarding enabled
☐ NAT/masquerade configured
☐ MTU settings
☐ DNS configuration
☐ Application-specific issues
```



---

## Behavioral Questions

### Q31: Tell me about a challenging networking bug you debugged.
**Sample Answer Structure:**

**Situation:**
"In my previous role, we had intermittent packet loss affecting production traffic. Users reported slow performance, but it was hard to reproduce."

**Task:**
"I was tasked with identifying and resolving the issue within 24 hours as it was impacting customers."

**Action:**
1. Started with tcpdump captures on both client and server
2. Noticed TCP retransmissions but only for large transfers
3. Suspected MTU issue
4. Used ping with different packet sizes to confirm
5. Discovered ISP was silently dropping packets >1400 bytes
6. Implemented path MTU discovery
7. Configured MSS clamping on router

**Result:**
"Packet loss dropped to 0%, customer complaints ceased. Created documentation for team on debugging MTU issues."

**Learning:**
"Learned importance of systematic debugging and not assuming common MTU values."

### Q32: Why do you want to work on protocol stack engineering?
**Key Points to Mention:**

✅ **Technical Interest:**
- Fascination with low-level networking
- Enjoy performance optimization
- Interest in security (VPNs, encryption)

✅ **Impact:**
- Core infrastructure affects millions
- Challenging technical problems
- Continuous learning (new protocols)

✅ **Specific Interest in this Role:**
- IPsec and WireGuard expertise
- C/C++ systems programming
- Linux kernel networking

**Sample Answer:**
"I'm passionate about systems-level programming and networking. Working at the protocol stack level combines both. I find it intellectually satisfying to optimize packet processing, implement security protocols, and see tangible performance improvements. The role offers challenges in cryptography, concurrency, and performance engineering - all areas I want to deepen my expertise in."

### Q33: How do you stay updated with networking technologies?
**Good Answer:**

"I follow multiple channels:
1. **RFCs and Standards:** Read IETF RFCs for new protocols
2. **Kernel Mailing Lists:** Follow netdev for Linux networking changes
3. **Blogs:** Julia Evans, Brendan Gregg for systems/networking insights
4. **Open Source:** Contribute to projects like WireGuard, study code
5. **Conferences:** Watch talks from Netdev, NANOG (recordings)
6. **Hands-on:** Lab environment where I test new technologies
7. **Papers:** Read research on congestion control, protocol design"



---

## Bonus: Quick Fire Round

### Q34: What port does HTTPS use?
**A:** 443

### Q35: What is the size of IPv4 address?
**A:** 32 bits (4 bytes)

### Q36: Default TTL for Linux?
**A:** 64

### Q37: Maximum TCP window size?
**A:** 65,535 bytes (without window scaling), ~1GB (with window scaling)

### Q38: UDP header size?
**A:** 8 bytes

### Q39: TCP header minimum size?
**A:** 20 bytes

### Q40: Ethernet frame minimum size?
**A:** 64 bytes (including header and FCS)

### Q41: What is the IP protocol number for TCP?
**A:** 6

### Q42: What is the IP protocol number for UDP?
**A:** 17

### Q43: What is the IP protocol number for ICMP?
**A:** 1

### Q44: What is the IP protocol number for ESP (IPsec)?
**A:** 50

### Q45: Default IPsec/IKE port?
**A:** UDP 500 (IKE), UDP 4500 (NAT-T)

### Q46: Default WireGuard port?
**A:** UDP 51820 (configurable)

### Q47: What is AES block size?
**A:** 128 bits (16 bytes)

### Q48: What is Curve25519 key size?
**A:** 256 bits (32 bytes)

### Q49: Maximum sequence number in TCP?
**A:** 2^32 - 1 (4,294,967,295)

### Q50: What is TIME_WAIT duration?
**A:** 2 * MSL (Maximum Segment Lifetime), typically 60-120 seconds

---

## Summary & Tips

### Interview Success Formula:

**1. Conceptual Understanding:**
- Explain WHY, not just WHAT
- Draw diagrams freely
- Think layer by layer

**2. Practical Experience:**
- Mention tools you've used
- Discuss real debugging scenarios
- Show hands-on knowledge

**3. Communication:**
- Think out loud
- Ask clarifying questions
- Structure your answers

**4. Depth:**
- Start high-level, then drill down
- Show you can go deep when asked
- Connect concepts together

### Common Interview Flow:

```
1. Introduction & Background (10 min)
   - Your experience
   - Why this role

2. Technical Questions (40-50 min)
   - Fundamentals (TCP/IP stack)
   - IPsec/WireGuard specifics
   - Coding problem (maybe)
   - System design

3. Troubleshooting Scenario (15 min)
   - Real-world problem
   - Your debugging approach

4. Your Questions (10 min)
   - Team structure
   - Technology stack
   - Projects you'd work on

5. Wrap-up (5 min)
```

### What to Prepare:

✅ Draw OSI model from memory  
✅ Explain TCP 3-way handshake with sequence numbers  
✅ Compare IPsec and WireGuard  
✅ Write socket code (client/server)  
✅ Debug packet loss scenario  
✅ Explain checksum calculation  
✅ Describe routing decision process  
✅ Know your resume projects deeply  

### Red Flags to Avoid:

❌ "I don't know" without trying  
❌ Making up answers  
❌ Not asking questions  
❌ Inability to explain past work  
❌ No practical experience  

### Good Luck! 🚀

Remember:
- Be honest about what you know and don't know
- Show enthusiasm for learning
- Demonstrate problem-solving ability
- Connect theory to practice

**You've got this!**
