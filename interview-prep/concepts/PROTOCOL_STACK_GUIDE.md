# Networking Protocol Stack - Complete Guide

## Table of Contents
1. [OSI Model & TCP/IP Stack](#osi-model--tcpip-stack)
2. [Layer 1 - Physical Layer](#layer-1---physical-layer)
3. [Layer 2 - Data Link Layer](#layer-2---data-link-layer)
4. [Layer 3 - Network Layer](#layer-3---network-layer)
5. [Layer 4 - Transport Layer](#layer-4---transport-layer)
6. [Layer 5-7 - Application Layers](#layer-5-7---application-layers)
7. [Protocol Stack Implementation](#protocol-stack-implementation)
8. [Packet Journey](#packet-journey)
9. [Interview Questions](#interview-questions)

---

## OSI Model & TCP/IP Stack

### OSI Model (7 Layers) vs TCP/IP Model (4 Layers)

```
┌─────────────────────────┬──────────────────────────┐
│     OSI Model           │    TCP/IP Model          │
├─────────────────────────┼──────────────────────────┤
│ 7. Application          │                          │
│ 6. Presentation         │  4. Application          │
│ 5. Session              │                          │
├─────────────────────────┼──────────────────────────┤
│ 4. Transport            │  3. Transport            │
├─────────────────────────┼──────────────────────────┤
│ 3. Network              │  2. Internet             │
├─────────────────────────┼──────────────────────────┤
│ 2. Data Link            │  1. Network Access       │
│ 1. Physical             │     (Link Layer)         │
└─────────────────────────┴──────────────────────────┘
```

### Mnemonic for OSI Layers:
**"All People Seem To Need Data Processing"**
- **A**pplication
- **P**resentation
- **S**ession
- **T**ransport
- **N**etwork
- **D**ata Link
- **P**hysical

---

## Layer 1 - Physical Layer

### Purpose:
Transmission of raw bits over physical medium

### Responsibilities:
- Bit transmission (0s and 1s)
- Physical connectors and cables
- Voltage levels and timing
- Bit rate and synchronization



### Technologies:
- **Wired**: Ethernet cables (Cat5, Cat6), Fiber optic
- **Wireless**: Wi-Fi, Bluetooth, Cellular (4G, 5G)
- **Standards**: IEEE 802.3 (Ethernet), IEEE 802.11 (Wi-Fi)

### Key Concepts:
- **Bandwidth**: Maximum data rate (e.g., 1 Gbps)
- **Latency**: Time for signal to travel
- **Encoding**: NRZ, Manchester, 4B/5B
- **Modulation**: AM, FM, QAM

---

## Layer 2 - Data Link Layer

### Purpose:
Reliable data transfer between directly connected nodes

### Sublayers:
1. **LLC (Logical Link Control)**: Flow control, error checking
2. **MAC (Media Access Control)**: Physical addressing, channel access

### Ethernet Frame Structure:

```
┌───────────────┬─────────┬─────────┬──────────┬─────────┬─────┐
│  Preamble     │  Dest   │  Source │   Type   │ Payload │ FCS │
│  (7 bytes)    │  MAC    │   MAC   │ (2 bytes)│ (Data)  │(CRC)│
│  + SFD (1B)   │ (6 B)   │  (6 B)  │          │         │(4B) │
└───────────────┴─────────┴─────────┴──────────┴─────────┴─────┘
        8            6         6         2       46-1500    4

Total: 64 to 1518 bytes (without preamble)
MTU (Maximum Transmission Unit): 1500 bytes
```



### MAC Address:
- **Size**: 48 bits (6 bytes)
- **Format**: XX:XX:XX:XX:XX:XX (hex)
- **Example**: 00:1A:2B:3C:4D:5E
- **Parts**: 
  - First 3 bytes: OUI (Organizationally Unique Identifier) - Vendor
  - Last 3 bytes: Device-specific

### Key Protocols:
- **Ethernet**: IEEE 802.3
- **Wi-Fi**: IEEE 802.11
- **PPP**: Point-to-Point Protocol
- **ARP**: Address Resolution Protocol (maps IP to MAC)

### ARP (Address Resolution Protocol):

```
Scenario: Host A (10.0.0.1) wants to send to Host B (10.0.0.2)
but doesn't know B's MAC address

1. A broadcasts ARP Request:
   "Who has 10.0.0.2? Tell 00:11:22:33:44:55"
   
2. B replies with ARP Response:
   "10.0.0.2 is at AA:BB:CC:DD:EE:FF"
   
3. A caches this in ARP table and sends data
```

### Switching:
- **Hub** (Layer 1): Broadcasts to all ports (collision domain)
- **Switch** (Layer 2): Forwards to specific port (MAC table)
- **Bridge**: Connects two network segments



---

## Layer 3 - Network Layer

### Purpose:
Routing packets across networks, logical addressing

### IPv4 Header Structure:

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |Flags|      Fragment Offset    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |         Header Checksum       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source IP Address                       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination IP Address                     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options (if IHL > 5)                       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Minimum Header: 20 bytes
Maximum Header: 60 bytes (with options)
```

### Key Fields:
- **Version**: 4 for IPv4
- **IHL**: Header length in 32-bit words (min 5)
- **TTL**: Hop limit (decremented at each router)
- **Protocol**: Upper layer protocol (6=TCP, 17=UDP, 1=ICMP)
- **Checksum**: Header integrity check



### IPv4 Addressing:

```
Classes (Historical):
Class A: 0.0.0.0     to 127.255.255.255   (1st bit: 0)
Class B: 128.0.0.0   to 191.255.255.255   (1st bits: 10)
Class C: 192.0.0.0   to 223.255.255.255   (1st bits: 110)
Class D: 224.0.0.0   to 239.255.255.255   (Multicast)
Class E: 240.0.0.0   to 255.255.255.255   (Reserved)

Private Ranges (RFC 1918):
10.0.0.0/8        (10.0.0.0 - 10.255.255.255)
172.16.0.0/12     (172.16.0.0 - 172.31.255.255)
192.168.0.0/16    (192.168.0.0 - 192.168.255.255)

Special Addresses:
127.0.0.0/8       (Loopback)
0.0.0.0           (Default route)
255.255.255.255   (Broadcast)
```

### CIDR (Classless Inter-Domain Routing):

```
Example: 192.168.1.0/24

/24 means first 24 bits are network, last 8 bits are host

Network:   192.168.1.0
Netmask:   255.255.255.0
Usable:    192.168.1.1 - 192.168.1.254
Broadcast: 192.168.1.255
Total IPs: 256 (254 usable)
```

### Subnetting Example:

```
Given: 192.168.1.0/24, divide into 4 subnets

/24 → /26 (4 subnets, 64 IPs each)

Subnet 1: 192.168.1.0/26   (.0 - .63)
Subnet 2: 192.168.1.64/26  (.64 - .127)
Subnet 3: 192.168.1.128/26 (.128 - .191)
Subnet 4: 192.168.1.192/26 (.192 - .255)
```



### Routing:

**Routing Table Example:**
```
Destination     Gateway         Netmask         Interface
0.0.0.0         192.168.1.1     0.0.0.0         eth0        (Default)
192.168.1.0     0.0.0.0         255.255.255.0   eth0        (Direct)
10.0.0.0        192.168.1.254   255.0.0.0       eth0        (Static)
```

**Routing Decision:**
```
1. Check destination IP
2. Match against routing table (longest prefix match)
3. Forward to next hop or deliver locally
4. Decrement TTL
5. Recalculate checksum
```

### ICMP (Internet Control Message Protocol):

```
Common ICMP Types:
Type 0:  Echo Reply (ping response)
Type 3:  Destination Unreachable
Type 5:  Redirect
Type 8:  Echo Request (ping)
Type 11: Time Exceeded (TTL=0)

Ping Process:
Client ──── ICMP Echo Request (Type 8) ────► Server
Client ◄─── ICMP Echo Reply (Type 0) ─────── Server
```

### NAT (Network Address Translation):

```
Private Network          Public Network
(192.168.1.0/24)        (1.2.3.4)

Client: 192.168.1.10:5000 ──┐
                             │ NAT Gateway
Server: 8.8.8.8:80 ◄─────────┘
                    (Sees 1.2.3.4:12345)

NAT Table:
Internal              External           Destination
192.168.1.10:5000 ←→ 1.2.3.4:12345  ←→ 8.8.8.8:80
```



---

## Layer 4 - Transport Layer

### Purpose:
End-to-end communication, reliability, flow control

### TCP (Transmission Control Protocol)

#### TCP Header Structure:

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Sequence Number                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Acknowledgment Number                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Data |           |U|A|P|R|S|F|                               |
| Offset| Reserved  |R|C|S|S|Y|I|            Window             |
|       |           |G|K|H|T|N|N|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           Checksum            |         Urgent Pointer        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options (if Data Offset > 5)               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Minimum Header: 20 bytes
Maximum Header: 60 bytes (with options)
```

#### TCP Flags:
- **SYN**: Synchronize (connection establishment)
- **ACK**: Acknowledgment
- **FIN**: Finish (connection termination)
- **RST**: Reset (abort connection)
- **PSH**: Push (deliver data immediately)
- **URG**: Urgent (urgent data pointer valid)



#### TCP 3-Way Handshake:

```
Client                                Server
------                                ------

SYN (seq=100) ───────────────────────►
                                      (Allocate resources)
                 ◄──────────────────── SYN-ACK (seq=300, ack=101)
                                      
ACK (seq=101, ack=301) ──────────────►
                                      
[Connection ESTABLISHED]

Client ISN: 100
Server ISN: 300
```

#### TCP Connection Termination (4-Way):

```
Client                                Server
------                                ------

FIN (seq=500) ───────────────────────►
                                      (Half-close)
                 ◄──────────────────── ACK (ack=501)
                                      
                 ◄──────────────────── FIN (seq=800)
                                      
ACK (ack=801) ───────────────────────►
                                      
[Connection CLOSED]
```

#### TCP State Machine:

```
CLOSED → LISTEN (server)
CLOSED → SYN_SENT → ESTABLISHED (client)
LISTEN → SYN_RECEIVED → ESTABLISHED (server)
ESTABLISHED → FIN_WAIT_1 → FIN_WAIT_2 → TIME_WAIT → CLOSED
ESTABLISHED → CLOSE_WAIT → LAST_ACK → CLOSED
```



#### TCP Features:

**1. Reliability (ARQ - Automatic Repeat Request):**
```
Sender                    Receiver
------                    --------
Data (seq=100, len=50) ──►
                         ◄── ACK (ack=150)

Data (seq=150, len=50) ──► [LOST]
                         
[Timeout - Retransmit]
Data (seq=150, len=50) ──►
                         ◄── ACK (ack=200)
```

**2. Flow Control (Sliding Window):**
```
Receiver advertises window size in ACKs
Sender cannot send more than receiver's window

Example:
Receiver Window = 4000 bytes
Sender can send up to 4000 bytes before ACK
Window shrinks as data sent, grows when ACKed
```

**3. Congestion Control:**
```
Algorithms:
- Slow Start: Exponential growth
- Congestion Avoidance: Linear growth
- Fast Retransmit: Duplicate ACKs trigger retransmit
- Fast Recovery: After fast retransmit

Congestion Window (cwnd):
Start: 1 MSS (Maximum Segment Size)
Grows: 1 MSS per ACK (slow start)
Then: Linear growth (congestion avoidance)
On loss: Reduce to half (multiplicative decrease)
```



### UDP (User Datagram Protocol)

#### UDP Header Structure:

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            Length             |           Checksum            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          Data ...                             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Header: 8 bytes (fixed)
```

#### UDP Characteristics:
- ✅ Connectionless (no handshake)
- ✅ Fast (low overhead)
- ✅ Lightweight (8-byte header)
- ❌ Unreliable (no ACKs)
- ❌ No ordering
- ❌ No flow control
- ❌ No congestion control

#### When to Use UDP:
- Real-time applications (VoIP, gaming, streaming)
- DNS queries
- DHCP
- Broadcast/multicast
- When speed > reliability

### TCP vs UDP Comparison:

| Feature | TCP | UDP |
|---------|-----|-----|
| Connection | Connection-oriented | Connectionless |
| Reliability | Guaranteed delivery | No guarantee |
| Ordering | In-order delivery | No ordering |
| Speed | Slower (overhead) | Faster |
| Header Size | 20-60 bytes | 8 bytes |
| Flow Control | Yes | No |
| Congestion Control | Yes | No |
| Use Cases | HTTP, FTP, SSH, Email | DNS, VoIP, Streaming |



---

## Layer 5-7 - Application Layers

### Session Layer (Layer 5)
- Manages sessions/connections
- Synchronization
- Dialog control
- Examples: NetBIOS, RPC

### Presentation Layer (Layer 6)
- Data format conversion
- Encryption/decryption
- Compression
- Examples: SSL/TLS, JPEG, ASCII

### Application Layer (Layer 7)
- User-facing protocols
- Network services

#### Common Application Protocols:

**HTTP/HTTPS (Web):**
```
Port: 80 (HTTP), 443 (HTTPS)
Request:
GET /index.html HTTP/1.1
Host: example.com

Response:
HTTP/1.1 200 OK
Content-Type: text/html
```

**DNS (Domain Name System):**
```
Port: 53 (UDP/TCP)
Query: example.com → ?
Response: example.com → 93.184.216.34
```

**FTP (File Transfer):**
```
Control: Port 21 (commands)
Data: Port 20 (file transfer)
```

**SSH (Secure Shell):**
```
Port: 22
Encrypted remote access
```

**SMTP/POP3/IMAP (Email):**
```
SMTP: Port 25/587 (sending)
POP3: Port 110 (receiving)
IMAP: Port 143 (receiving)
```



---

## Protocol Stack Implementation

### Kernel Space vs User Space:

```
┌─────────────────────────────────────┐
│         User Space                  │
│  ┌──────────────────────────────┐  │
│  │   Applications (HTTP, SSH)   │  │
│  └──────────────┬───────────────┘  │
│                 │ System Calls      │
│                 │ (socket, send)    │
├─────────────────┼───────────────────┤
│         Kernel Space                │
│  ┌──────────────▼───────────────┐  │
│  │   Socket Layer                │  │
│  ├──────────────────────────────┤  │
│  │   Transport (TCP/UDP)        │  │
│  ├──────────────────────────────┤  │
│  │   Network (IP, ICMP, IPsec)  │  │
│  ├──────────────────────────────┤  │
│  │   Data Link (Ethernet, ARP)  │  │
│  ├──────────────────────────────┤  │
│  │   Network Device Drivers     │  │
│  └──────────────┬───────────────┘  │
│                 │                   │
└─────────────────┼───────────────────┘
                  │
          ┌───────▼────────┐
          │  Hardware NIC  │
          └────────────────┘
```

### Socket API (Berkeley Sockets):

**TCP Server:**
```c
socket()    // Create socket
bind()      // Bind to address:port
listen()    // Mark as passive (server)
accept()    // Wait for client connection
recv()      // Receive data
send()      // Send data
close()     // Close connection
```

**TCP Client:**
```c
socket()    // Create socket
connect()   // Connect to server
send()      // Send data
recv()      // Receive data
close()     // Close connection
```

**UDP:**
```c
socket()    // Create socket
bind()      // Bind to port (optional for client)
sendto()    // Send datagram
recvfrom()  // Receive datagram
close()     // Close socket
```



### Linux Network Stack Architecture:

```
Application
    ↓
System Call (read/write/sendmsg/recvmsg)
    ↓
Socket Layer (struct socket)
    ↓
Protocol Layer (TCP/UDP - struct proto)
    ↓
IP Layer (routing, fragmentation)
    ↓
Netfilter (iptables hooks)
    ↓
Device Layer (struct net_device)
    ↓
Driver (e1000, ixgbe)
    ↓
Hardware (NIC)
```

### Packet Buffer (sk_buff):

```c
struct sk_buff {
    // Packet data
    unsigned char *head;    // Start of allocated buffer
    unsigned char *data;    // Start of actual data
    unsigned char *tail;    // End of actual data
    unsigned char *end;     // End of allocated buffer
    
    // Metadata
    struct net_device *dev; // Network device
    struct sock *sk;        // Socket
    unsigned int len;       // Data length
    
    // Protocol headers
    struct tcphdr *th;
    struct iphdr *iph;
    struct ethhdr *eth;
    
    // Timestamps, checksums, etc.
};
```

### Netfilter Hooks (iptables):

```
┌──────────────────────────────────────┐
│         Incoming Packet              │
└──────────────┬───────────────────────┘
               │
               ▼
      ┌────────────────┐
      │  PREROUTING     │ (NAT, mangle)
      └────────┬────────┘
               │
        ┌──────▼──────┐
        │   Routing   │
        │  Decision   │
        └──────┬──────┘
               │
       ┌───────┴────────┐
       │                │
Local  │            Forward
Dest   │                │
       ▼                ▼
 ┌──────────┐    ┌──────────┐
 │  INPUT   │    │ FORWARD  │ (filter)
 └────┬─────┘    └────┬─────┘
      │               │
      ▼               ▼
 Local Process   ┌──────────┐
      │          │POSTROUTING│ (NAT)
      │          └────┬─────┘
      ▼               │
 ┌──────────┐        │
 │  OUTPUT  │        │
 └────┬─────┘        │
      │              │
      └──────┬───────┘
             ▼
       Send to wire
```



---

## Packet Journey

### Complete HTTP Request Flow:

**Scenario:** Browser requests http://example.com

#### Application Layer (Layer 7):
```
1. Browser creates HTTP request:
   GET / HTTP/1.1
   Host: example.com
```

#### Transport Layer (Layer 4):
```
2. DNS lookup: example.com → 93.184.216.34
3. Socket created, TCP connection initiated
4. TCP 3-way handshake:
   SYN → SYN-ACK → ACK
5. HTTP request data given to TCP
6. TCP segments data, adds TCP header:
   - Source Port: 54321 (random)
   - Dest Port: 80
   - Seq, ACK numbers
   - Flags, Window, Checksum
```

#### Network Layer (Layer 3):
```
7. TCP segment given to IP layer
8. IP header added:
   - Source IP: 192.168.1.10 (your IP)
   - Dest IP: 93.184.216.34
   - Protocol: 6 (TCP)
   - TTL: 64
   - Checksum calculated
9. Routing decision:
   - Destination not local
   - Use default gateway: 192.168.1.1
```

#### Data Link Layer (Layer 2):
```
10. ARP lookup for gateway MAC:
    192.168.1.1 → AA:BB:CC:DD:EE:FF
11. Ethernet frame created:
    - Dest MAC: AA:BB:CC:DD:EE:FF (gateway)
    - Source MAC: 00:11:22:33:44:55 (your NIC)
    - Type: 0x0800 (IPv4)
    - Payload: IP packet
    - FCS: CRC checksum
```

#### Physical Layer (Layer 1):
```
12. Frame converted to electrical signals
13. Transmitted on network cable
```



### Packet at Each Layer (Encapsulation):

```
┌──────────────────────────────────────────────────────────┐
│                    Application Layer                      │
│  HTTP Request: "GET / HTTP/1.1\r\nHost: example.com\r\n" │
└──────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────┐
│                    Transport Layer                        │
│  ┌──────────────┬───────────────────────────────────┐   │
│  │  TCP Header  │  HTTP Data                        │   │
│  └──────────────┴───────────────────────────────────┘   │
└──────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────┐
│                     Network Layer                         │
│  ┌─────────────┬──────────────┬────────────────────┐    │
│  │  IP Header  │  TCP Header  │  HTTP Data         │    │
│  └─────────────┴──────────────┴────────────────────┘    │
└──────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────┐
│                   Data Link Layer                         │
│  ┌────────┬──────┬──────┬─────────┬─────────────┬────┐  │
│  │Ethernet│ IP   │ TCP  │  HTTP   │ (Payload)   │FCS │  │
│  │ Header │Header│Header│  Data   │             │    │  │
│  └────────┴──────┴──────┴─────────┴─────────────┴────┘  │
└──────────────────────────────────────────────────────────┘
                            ↓
                    Physical Layer
               (Electrical/Optical Signals)
```

### Receiving Side (Decapsulation):

```
Physical → Data Link → Network → Transport → Application
   
1. NIC receives electrical signals
2. Data Link: Check FCS, extract IP packet
3. Network: Check IP header, checksum, TTL
4. Transport: TCP processing, check checksum, ACK
5. Application: Deliver HTTP data to browser
```



---

## Interview Questions

### Q1: Explain the OSI model layers.
**A:** 7 layers from bottom to top:
1. Physical - bits on wire
2. Data Link - MAC addressing, frames
3. Network - IP addressing, routing
4. Transport - TCP/UDP, end-to-end
5. Session - connection management
6. Presentation - encoding, encryption
7. Application - user protocols (HTTP, etc.)

### Q2: What happens when you type a URL in browser?
**A:**
1. DNS lookup (domain → IP)
2. TCP 3-way handshake (SYN, SYN-ACK, ACK)
3. TLS handshake (if HTTPS)
4. HTTP request sent
5. Server processes, sends response
6. Browser renders page
7. TCP connection closed (4-way FIN)

### Q3: Difference between hub, switch, and router?
**A:**
- **Hub** (L1): Broadcasts to all ports, half-duplex, collision domain
- **Switch** (L2): Forwards to specific port based on MAC, full-duplex
- **Router** (L3): Routes between networks based on IP, different subnets

### Q4: How does TCP ensure reliability?
**A:**
- Sequence numbers (ordering)
- Acknowledgments (receipt confirmation)
- Retransmission on timeout
- Checksums (detect corruption)
- Flow control (window size)

### Q5: What is the difference between TCP and UDP?
**A:** TCP is connection-oriented, reliable, ordered with overhead. UDP is connectionless, unreliable, fast with minimal overhead. Use TCP for accuracy (HTTP, FTP), UDP for speed (VoIP, gaming, DNS).



### Q6: Explain the TCP 3-way handshake.
**A:**
1. Client → Server: SYN (seq=x)
2. Server → Client: SYN-ACK (seq=y, ack=x+1)
3. Client → Server: ACK (ack=y+1)
Connection established. Both sides have synchronized sequence numbers.

### Q7: What is TTL and why is it needed?
**A:** Time To Live - hop limit in IP packet. Decremented at each router. When 0, packet is dropped and ICMP "Time Exceeded" sent back. Prevents infinite routing loops.

### Q8: How does ARP work?
**A:** Maps IP to MAC address. Host broadcasts "Who has IP X?" on local network. Device with IP X replies "I have IP X, my MAC is Y". Requestor caches this mapping.

### Q9: What is NAT and why is it used?
**A:** Network Address Translation maps private IPs to public IP(s). Needed because IPv4 addresses are scarce. Allows multiple devices to share one public IP. Also provides basic security by hiding internal topology.

### Q10: Explain TCP congestion control.
**A:**
- **Slow Start**: Exponential growth of congestion window
- **Congestion Avoidance**: Linear growth after threshold
- **Fast Retransmit**: 3 duplicate ACKs trigger immediate retransmit
- **Fast Recovery**: Multiplicative decrease on loss
Goal: Maximize throughput while avoiding network congestion

### Q11: What is the maximum size of a TCP segment?
**A:** Determined by MSS (Maximum Segment Size), which is derived from MTU:
- Ethernet MTU: 1500 bytes
- Minus IP header: 20 bytes
- Minus TCP header: 20 bytes
- MSS: 1460 bytes (typical)

### Q12: How does sliding window work?
**A:** Receiver advertises window size in ACKs. Sender can send up to that amount before waiting for ACK. As data is ACKed, window "slides" forward allowing more data to be sent. Provides flow control.



### Q13: What is the purpose of checksums in TCP/UDP?
**A:** Detect data corruption during transmission. Calculated over pseudo-header (src/dst IP, protocol, length) + actual header + data. Receiver recalculates and compares. If mismatch, packet is dropped.

### Q14: Explain packet fragmentation.
**A:** When packet exceeds network MTU, it's fragmented into smaller pieces. Each fragment has same ID, different offset. DF (Don't Fragment) flag prevents this. Receiver reassembles using ID and offset. Used in Path MTU Discovery.

### Q15: What are socket states in TCP?
**A:** 
- **LISTEN**: Server waiting for connections
- **SYN_SENT**: Client sent SYN, awaiting SYN-ACK
- **SYN_RECEIVED**: Server received SYN, sent SYN-ACK
- **ESTABLISHED**: Connection active, data transfer
- **FIN_WAIT_1/2**: Active close in progress
- **CLOSE_WAIT**: Passive close, waiting for app to close
- **TIME_WAIT**: Wait before final close (2*MSL)
- **CLOSED**: No connection

### Q16: Why is there a TIME_WAIT state?
**A:** After connection closes, socket stays in TIME_WAIT for 2*MSL (Maximum Segment Lifetime, typically 60-120 seconds). Ensures:
1. All packets from old connection are gone
2. Final ACK can be retransmitted if FIN is resent
3. Prevents old segments from corrupting new connections

### Q17: How does a protocol stack handle incoming packets?
**A:**
1. NIC receives, DMAs to memory
2. Interrupt/NAPI notifies kernel
3. Data Link: Process Ethernet, check FCS
4. Network: Process IP, check checksum, TTL, routing
5. Transport: Process TCP/UDP, check checksum
6. Deliver to socket buffer
7. Application reads via recv/read

---

## Performance Considerations

### Zero-Copy Techniques:
- **sendfile()**: Transfer data between file descriptors without copying to userspace
- **mmap()**: Map file directly to memory
- **splice()**: Move data between pipes/sockets
- **DMA**: Direct Memory Access by NIC

### Offloading:
- **TSO (TCP Segmentation Offload)**: NIC splits large packets
- **GSO (Generic Segmentation Offload)**: Software version of TSO
- **LRO/GRO**: Large/Generic Receive Offload - merge packets
- **Checksum Offload**: NIC calculates checksums

### High-Performance Networking:
- **DPDK (Data Plane Development Kit)**: Bypass kernel, userspace packet processing
- **XDP (eXpress Data Path)**: eBPF in NIC driver
- **io_uring**: Async I/O interface
- **AF_XDP**: Zero-copy socket

---

## Summary

**Key Takeaways:**

1. **Encapsulation**: Each layer adds header, wrapping previous layer
2. **Decapsulation**: Receiving side removes headers layer by layer
3. **TCP vs UDP**: Reliability vs Speed trade-off
4. **IP Routing**: Longest prefix match, hop-by-hop
5. **Protocol Stack**: Kernel implements layers 2-4, apps do layer 7

**For Interview Success:**
- Draw the OSI model from memory
- Explain TCP 3-way handshake with sequence numbers
- Describe complete packet flow from app to wire
- Know when to use TCP vs UDP
- Understand NAT, routing, and ARP

Good luck! 🚀
