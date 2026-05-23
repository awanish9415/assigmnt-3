/*
 * Problem 10: IPsec ESP (Encapsulating Security Payload) Packet Parser
 * 
 * Description:
 * Parse IPsec ESP packet structure and understand encryption/authentication.
 * This is crucial for Protocol Stack Engineer role.
 * 
 * Key Concepts:
 * - IPsec ESP header format
 * - Security Parameter Index (SPI)
 * - Sequence numbers
 * - Encryption and authentication
 * - Tunnel vs Transport mode
 * 
 * Interview Topics:
 * - What is IPsec and why is it used?
 * - ESP vs AH (Authentication Header)
 * - How does key exchange work? (IKE)
 * - Anti-replay protection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

/*
 * ESP Packet Format:
 * 
 * +------------------+
 * |  SPI (4 bytes)   |  Security Parameter Index
 * +------------------+
 * | Sequence (4 B)   |  Sequence Number
 * +------------------+  ← Authenticated and Encrypted ↓
 * | Payload Data     |  Encrypted (variable)
 * |     ...          |
 * +------------------+
 * | Padding (0-255)  |  Padding for block cipher
 * +------------------+
 * | Pad Len (1 B)    |  Padding length
 * +------------------+
 * | Next Header (1B) |  Protocol of encapsulated packet
 * +------------------+  ← Authenticated only ↓
 * | ICV (variable)   |  Integrity Check Value (HMAC)
 * +------------------+
 */

// ESP Header Structure
typedef struct {
    uint32_t spi;           // Security Parameter Index
    uint32_t sequence;      // Sequence Number
    // Followed by encrypted payload
} esp_header_t;

// ESP Trailer (at end of decrypted payload)
typedef struct {
    uint8_t pad_length;     // Length of padding
    uint8_t next_header;    // Next protocol (IP protocol number)
} esp_trailer_t;

// Security Association (SA) - simplified version
typedef struct {
    uint32_t spi;           // SPI for this SA
    char description[64];   // Human-readable description
    uint8_t encryption_algo;// Encryption algorithm
    uint8_t auth_algo;      // Authentication algorithm
    uint32_t seq_number;    // Last seen sequence number
    uint32_t replay_window; // Anti-replay window size
} security_association_t;

// Global SA Database (SAD) - simplified
#define MAX_SA 10
security_association_t sa_database[MAX_SA];
int sa_count = 0;

// Add SA to database
void add_sa(uint32_t spi, const char *desc, uint8_t enc_algo, uint8_t auth_algo) {
    if (sa_count >= MAX_SA) {
        printf("[ERROR] SA database full\n");
        return;
    }
    
    sa_database[sa_count].spi = spi;
    strncpy(sa_database[sa_count].description, desc, 63);
    sa_database[sa_count].encryption_algo = enc_algo;
    sa_database[sa_count].auth_algo = auth_algo;
    sa_database[sa_count].seq_number = 0;
    sa_database[sa_count].replay_window = 64;  // Default window
    sa_count++;
    
    printf("[+] Added SA: SPI=0x%08X, Desc=%s\n", spi, desc);
}

// Lookup SA by SPI
security_association_t* lookup_sa(uint32_t spi) {
    for (int i = 0; i < sa_count; i++) {
        if (sa_database[i].spi == spi) {
            return &sa_database[i];
        }
    }
    return NULL;
}

// Check for replay attack
int check_replay(security_association_t *sa, uint32_t seq) {
    // Simplified replay check
    if (seq == 0) {
        return 0;  // Invalid sequence number
    }
    
    if (seq <= sa->seq_number) {
        // Check if it's within replay window
        uint32_t diff = sa->seq_number - seq;
        if (diff < sa->replay_window) {
            printf("[WARNING] Potential replay attack! Old sequence: %u, current: %u\n",
                   seq, sa->seq_number);
            return 0;
        }
        printf("[ERROR] Sequence number too old (outside window)\n");
        return 0;
    }
    
    // Update sequence number
    sa->seq_number = seq;
    return 1;
}

// Get algorithm name
const char* get_encryption_name(uint8_t algo) {
    switch (algo) {
        case 1: return "NULL (No Encryption)";
        case 2: return "DES-CBC";
        case 3: return "3DES-CBC";
        case 12: return "AES-CBC";
        case 13: return "AES-CTR";
        case 14: return "AES-GCM";
        case 18: return "ChaCha20-Poly1305";
        default: return "Unknown";
    }
}

const char* get_auth_name(uint8_t algo) {
    switch (algo) {
        case 1: return "HMAC-MD5";
        case 2: return "HMAC-SHA1";
        case 5: return "HMAC-SHA2-256";
        case 6: return "HMAC-SHA2-384";
        case 7: return "HMAC-SHA2-512";
        default: return "Unknown";
    }
}

// Parse ESP packet
void parse_esp_packet(uint8_t *packet, int packet_len, int is_encrypted) {
    if (packet_len < sizeof(esp_header_t)) {
        printf("[ERROR] Packet too small for ESP header\n");
        return;
    }
    
    esp_header_t *esp = (esp_header_t *)packet;
    uint32_t spi = ntohl(esp->spi);
    uint32_t seq = ntohl(esp->sequence);
    
    printf("\n========== ESP PACKET ANALYSIS ==========\n\n");
    printf("SPI (Security Parameter Index): 0x%08X (%u)\n", spi, spi);
    printf("Sequence Number: %u\n", seq);
    
    // Lookup SA
    security_association_t *sa = lookup_sa(spi);
    if (sa == NULL) {
        printf("[ERROR] No Security Association found for SPI 0x%08X\n", spi);
        printf("       Packet will be dropped!\n");
        return;
    }
    
    printf("\n[✓] Security Association Found:\n");
    printf("    Description: %s\n", sa->description);
    printf("    Encryption: %s\n", get_encryption_name(sa->encryption_algo));
    printf("    Authentication: %s\n", get_auth_name(sa->auth_algo));
    
    // Check replay
    if (!check_replay(sa, seq)) {
        printf("[✗] Replay check FAILED! Packet dropped.\n");
        return;
    }
    printf("[✓] Replay check PASSED\n");
    
    // Payload analysis
    int payload_offset = sizeof(esp_header_t);
    int payload_len = packet_len - payload_offset;
    
    if (is_encrypted) {
        printf("\nEncrypted Payload: %d bytes\n", payload_len);
        printf("   First 16 bytes: ");
        for (int i = 0; i < 16 && i < payload_len; i++) {
            printf("%02X ", packet[payload_offset + i]);
        }
        printf("\n");
        printf("\n[!] Packet is encrypted. Need to decrypt with SA keys.\n");
        printf("    Decryption process:\n");
        printf("    1. Verify ICV (Integrity Check Value)\n");
        printf("    2. Decrypt payload using encryption algorithm\n");
        printf("    3. Remove padding\n");
        printf("    4. Extract inner IP packet\n");
    } else {
        printf("\n[*] Simulating DECRYPTED payload...\n");
        // In real scenario, this would be after decryption
        printf("    Decrypted payload would contain inner IP packet\n");
    }
    
    printf("\n========================================\n");
}

// Create sample ESP packet
void create_sample_esp_packet(uint8_t *packet, uint32_t spi, uint32_t seq) {
    esp_header_t *esp = (esp_header_t *)packet;
    esp->spi = htonl(spi);
    esp->sequence = htonl(seq);
    
    // Simulate encrypted payload (random-looking data)
    for (int i = sizeof(esp_header_t); i < 100; i++) {
        packet[i] = rand() & 0xFF;
    }
}

// Demonstrate tunnel mode vs transport mode
void explain_modes() {
    printf("\n\n========== IPsec MODES ==========\n\n");
    
    printf("TRANSPORT MODE:\n");
    printf("---------------\n");
    printf("Used for end-to-end communication between two hosts\n");
    printf("Only the payload is encrypted, original IP header remains\n\n");
    printf("Packet Structure:\n");
    printf("[ Original IP Header ] [ ESP Header ] [ Encrypted TCP/UDP ] [ ESP Trailer ] [ ICV ]\n");
    printf("         ↑                                    ↑\n");
    printf("    Not encrypted                      Encrypted\n\n");
    
    printf("TUNNEL MODE:\n");
    printf("------------\n");
    printf("Used for site-to-site VPNs (gateway-to-gateway)\n");
    printf("Entire original packet is encrypted and new IP header added\n\n");
    printf("Packet Structure:\n");
    printf("[ New IP Header ] [ ESP Header ] [ Encrypted Original IP + Payload ] [ ESP Trailer ] [ ICV ]\n");
    printf("                                          ↑\n");
    printf("                               Entire original packet encrypted\n\n");
    
    printf("COMPARISON:\n");
    printf("-----------\n");
    printf("Transport Mode:\n");
    printf("  + Less overhead (no extra IP header)\n");
    printf("  + Better for host-to-host\n");
    printf("  - Source/dest IPs visible\n");
    printf("  - Doesn't work with NAT easily\n\n");
    
    printf("Tunnel Mode:\n");
    printf("  + Hides original source/dest\n");
    printf("  + Works with NAT\n");
    printf("  + Better for VPN gateways\n");
    printf("  - More overhead (extra IP header)\n\n");
}

int main() {
    printf("IPsec ESP Packet Parser\n");
    printf("=======================\n\n");
    
    // Initialize SA Database
    printf("[*] Initializing Security Association Database...\n\n");
    add_sa(0x12345678, "VPN to DataCenter", 12, 5);  // AES-CBC, HMAC-SHA256
    add_sa(0xABCDEF00, "Remote Office Link", 14, 6); // AES-GCM, HMAC-SHA384
    add_sa(0x11111111, "Test Connection", 3, 2);     // 3DES, HMAC-SHA1
    
    // Create and parse sample packets
    uint8_t packet1[100];
    create_sample_esp_packet(packet1, 0x12345678, 1001);
    parse_esp_packet(packet1, 100, 1);
    
    printf("\n\n--- Testing Sequence Numbers ---\n");
    uint8_t packet2[100];
    create_sample_esp_packet(packet2, 0x12345678, 1002);
    parse_esp_packet(packet2, 100, 1);
    
    printf("\n\n--- Testing Replay Attack Detection ---\n");
    uint8_t packet3[100];
    create_sample_esp_packet(packet3, 0x12345678, 1001);  // Old sequence
    parse_esp_packet(packet3, 100, 1);
    
    printf("\n\n--- Testing Unknown SPI ---\n");
    uint8_t packet4[100];
    create_sample_esp_packet(packet4, 0xDEADBEEF, 500);
    parse_esp_packet(packet4, 100, 1);
    
    // Explain modes
    explain_modes();
    
    return 0;
}

/*
 * COMPILATION:
 * gcc -o ipsec_esp_parser 10_ipsec_esp_parser.c
 * 
 * RUN:
 * ./ipsec_esp_parser
 * 
 * INTERVIEW QUESTIONS & ANSWERS:
 * 
 * Q1: What is IPsec and why is it used?
 * A: IPsec (Internet Protocol Security) is a protocol suite for securing
 *    IP communications through authentication and encryption. Used for:
 *    - VPNs (Virtual Private Networks)
 *    - Secure site-to-site communication
 *    - Protecting sensitive data over public networks
 *    - Works at network layer (Layer 3)
 * 
 * Q2: What is SPI (Security Parameter Index)?
 * A: SPI is a 32-bit identifier that, combined with destination IP and protocol,
 *    uniquely identifies a Security Association (SA). Receiver uses SPI to
 *    look up keys and algorithms for decryption/verification.
 * 
 * Q3: ESP vs AH - what's the difference?
 * A: ESP (Encapsulating Security Payload):
 *    - Provides confidentiality (encryption) + authentication
 *    - Encrypts payload
 *    - More commonly used
 *    - Protocol number: 50
 *    
 *    AH (Authentication Header):
 *    - Authentication only, NO encryption
 *    - Protects entire packet including IP header
 *    - Issues with NAT (modifies IP header)
 *    - Protocol number: 51
 *    - Rarely used today
 * 
 * Q4: How does anti-replay protection work?
 * A: Uses sequence numbers and replay window:
 *    - Each packet has unique, incrementing sequence number
 *    - Receiver maintains sliding window (e.g., 64 packets)
 *    - Packets with sequence numbers outside window are rejected
 *    - Prevents attackers from capturing and replaying old packets
 * 
 * Q5: What is IKE (Internet Key Exchange)?
 * A: Protocol for establishing SAs and exchanging keys:
 *    - IKEv1: Original version, complex, two phases
 *    - IKEv2: Modern, simplified, more secure
 *    - Uses Diffie-Hellman for key exchange
 *    - Authenticates peers (pre-shared keys or certificates)
 *    - Negotiates encryption/auth algorithms
 *    - Port: UDP 500, 4500 (for NAT-T)
 * 
 * Q6: What encryption algorithms does IPsec support?
 * A: Common ones:
 *    - 3DES (legacy, avoid)
 *    - AES-CBC (widely used)
 *    - AES-GCM (provides encryption + authentication)
 *    - AES-CTR
 *    - ChaCha20-Poly1305 (modern, fast)
 * 
 * Q7: What is Perfect Forward Secrecy (PFS)?
 * A: Property where compromise of long-term keys doesn't compromise
 *    past session keys. IPsec/IKE achieves this through Diffie-Hellman
 *    key exchange for each session.
 * 
 * Q8: How does IPsec work with NAT?
 * A: NAT-T (NAT Traversal):
 *    - Encapsulates ESP in UDP (port 4500)
 *    - Allows IPsec to work through NAT devices
 *    - Adds UDP header before ESP header
 *    - Required for ESP in transport mode behind NAT
 * 
 * Q9: What is Security Association (SA)?
 * A: One-way relationship between sender and receiver that defines:
 *    - SPI (identifier)
 *    - Encryption algorithm and keys
 *    - Authentication algorithm and keys
 *    - Sequence number counter
 *    - Lifetime
 *    - Mode (transport/tunnel)
 *    Need 2 SAs for bidirectional communication!
 * 
 * Q10: ESP header - what's encrypted vs authenticated?
 * A: Authenticated: ESP header + payload + ESP trailer
 *    Encrypted: Payload + ESP trailer
 *    IP header is never encrypted in transport mode
 * 
 * REAL-WORLD USAGE:
 * - Corporate VPNs (employee to office)
 * - Site-to-site VPNs (office to office)
 * - Cloud connectivity (AWS VPN, Azure VPN Gateway)
 * - IoT device security
 * - Mobile security (IKEv2 widely used for mobile VPNs)
 * 
 * COMMON ISSUES:
 * - NAT compatibility (use NAT-T)
 * - MTU issues (fragmentation before encryption)
 * - Firewall configuration (allow ESP, IKE ports)
 * - Key management complexity
 * - Performance overhead
 * 
 * MODERN ALTERNATIVES:
 * - WireGuard (simpler, faster, modern cryptography)
 * - TLS/SSL VPNs (layer 4/5)
 * - DTLS (Datagram TLS)
 */
