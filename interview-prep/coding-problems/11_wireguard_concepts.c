/*
 * Problem 11: WireGuard Protocol Concepts and Handshake Simulation
 * 
 * Description:
 * Understand WireGuard protocol - a modern, fast, and secure VPN protocol.
 * Simulate the handshake process and packet structure.
 * 
 * Key Concepts:
 * - Noise Protocol Framework
 * - Curve25519 for key exchange
 * - ChaCha20-Poly1305 for encryption
 * - Simpler than IPsec
 * - No configuration of cipher suites
 * 
 * Interview Topics:
 * - What makes WireGuard better than IPsec?
 * - How does the handshake work?
 * - Cryptographic primitives used
 * - Stateless nature
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/*
 * WireGuard Packet Types:
 * 
 * Type 1: Handshake Initiation (148 bytes)
 * Type 2: Handshake Response (92 bytes)
 * Type 3: Cookie Reply (64 bytes)
 * Type 4: Transport Data (variable, 16+ bytes overhead)
 */

#define WG_PACKET_HANDSHAKE_INITIATION  1
#define WG_PACKET_HANDSHAKE_RESPONSE    2
#define WG_PACKET_COOKIE_REPLY          3
#define WG_PACKET_DATA                  4

// WireGuard Handshake Initiation Message
typedef struct {
    uint8_t  type;              // Message type (1)
    uint8_t  reserved[3];       // Reserved (zeros)
    uint32_t sender_index;      // Sender's session index
    uint8_t  ephemeral[32];     // Ephemeral public key
    uint8_t  encrypted_static[48];   // Encrypted static public key + auth tag
    uint8_t  encrypted_timestamp[28]; // Encrypted timestamp + auth tag
    uint8_t  mac1[16];          // MAC of entire message
    uint8_t  mac2[16];          // Cookie-based MAC (optional)
} __attribute__((packed)) wg_handshake_init_t;

// WireGuard Handshake Response Message
typedef struct {
    uint8_t  type;              // Message type (2)
    uint8_t  reserved[3];       // Reserved (zeros)
    uint32_t sender_index;      // Sender's session index
    uint32_t receiver_index;    // Receiver's session index
    uint8_t  ephemeral[32];     // Ephemeral public key
    uint8_t  encrypted_nothing[16]; // Encrypted empty + auth tag
    uint8_t  mac1[16];          // MAC of entire message
    uint8_t  mac2[16];          // Cookie-based MAC (optional)
} __attribute__((packed)) wg_handshake_resp_t;

// WireGuard Transport Data Message
typedef struct {
    uint8_t  type;              // Message type (4)
    uint8_t  reserved[3];       // Reserved (zeros)
    uint32_t receiver_index;    // Receiver's session index
    uint64_t counter;           // Packet counter (nonce)
    uint8_t  encrypted_data[];  // Encrypted payload + 16 byte auth tag
} __attribute__((packed)) wg_data_packet_t;

// Peer information
typedef struct {
    char name[32];
    uint8_t public_key[32];
    uint8_t private_key[32];
    uint32_t session_index;
    uint64_t tx_counter;
    uint64_t rx_counter;
} wg_peer_t;

// Print bytes in hex
void print_hex(const char *label, uint8_t *data, int len) {
    printf("%s: ", label);
    for (int i = 0; i < len; i++) {
        printf("%02X", data[i]);
        if ((i + 1) % 32 == 0 && i != len - 1) printf("\n%*s", (int)strlen(label) + 2, "");
    }
    printf("\n");
}

// Generate random bytes (simplified - not cryptographically secure!)
void generate_random(uint8_t *buf, int len) {
    for (int i = 0; i < len; i++) {
        buf[i] = rand() & 0xFF;
    }
}

// Simulate key generation
void generate_keypair(uint8_t *private_key, uint8_t *public_key) {
    generate_random(private_key, 32);
    generate_random(public_key, 32);
    // In real WireGuard: public_key = Curve25519(private_key, base_point)
}

// Create peer
void create_peer(wg_peer_t *peer, const char *name) {
    strncpy(peer->name, name, 31);
    generate_keypair(peer->private_key, peer->public_key);
    peer->session_index = rand();
    peer->tx_counter = 0;
    peer->rx_counter = 0;
}

// Simulate handshake initiation
void create_handshake_initiation(wg_peer_t *initiator, wg_peer_t *responder, 
                                  wg_handshake_init_t *msg) {
    printf("\n========== HANDSHAKE INITIATION ==========\n\n");
    printf("[%s] Creating handshake initiation to [%s]\n\n", 
           initiator->name, responder->name);
    
    // Fill message
    memset(msg, 0, sizeof(wg_handshake_init_t));
    msg->type = WG_PACKET_HANDSHAKE_INITIATION;
    msg->sender_index = htonl(initiator->session_index);
    
    // Generate ephemeral keypair
    uint8_t ephemeral_private[32], ephemeral_public[32];
    generate_keypair(ephemeral_private, ephemeral_public);
    memcpy(msg->ephemeral, ephemeral_public, 32);
    
    // Simulate encryption of static key
    // Real: Encrypted with AEAD(ChaCha20-Poly1305)
    generate_random(msg->encrypted_static, 48);
    
    // Simulate encryption of timestamp
    generate_random(msg->encrypted_timestamp, 28);
    
    // Calculate MACs
    generate_random(msg->mac1, 16);
    generate_random(msg->mac2, 16);
    
    printf("Message Type: %d (Handshake Initiation)\n", msg->type);
    printf("Sender Index: 0x%08X\n", ntohl(msg->sender_index));
    print_hex("Ephemeral Public Key", msg->ephemeral, 32);
    printf("\n");
    printf("Encrypted Static Key: [48 bytes - encrypted with responder's public key]\n");
    printf("Encrypted Timestamp: [28 bytes - for replay protection]\n");
    printf("MAC1: [16 bytes - authenticates message]\n");
    printf("MAC2: [16 bytes - optional, for DoS protection]\n");
    
    printf("\nCryptographic Operations:\n");
    printf("1. Generate ephemeral keypair (Curve25519)\n");
    printf("2. Compute shared secret: ECDH(ephemeral_priv, responder_pub)\n");
    printf("3. Encrypt static public key with ChaCha20-Poly1305\n");
    printf("4. Encrypt current timestamp\n");
    printf("5. Compute MAC1 over entire message\n");
    
    printf("\nTotal Message Size: %lu bytes\n", sizeof(wg_handshake_init_t));
}

// Simulate handshake response
void create_handshake_response(wg_peer_t *responder, wg_handshake_init_t *init_msg,
                                wg_handshake_resp_t *resp_msg) {
    printf("\n========== HANDSHAKE RESPONSE ==========\n\n");
    printf("[%s] Creating handshake response\n\n", responder->name);
    
    memset(resp_msg, 0, sizeof(wg_handshake_resp_t));
    resp_msg->type = WG_PACKET_HANDSHAKE_RESPONSE;
    resp_msg->sender_index = htonl(responder->session_index);
    resp_msg->receiver_index = init_msg->sender_index;
    
    // Generate ephemeral keypair
    uint8_t ephemeral_private[32], ephemeral_public[32];
    generate_keypair(ephemeral_private, ephemeral_public);
    memcpy(resp_msg->ephemeral, ephemeral_public, 32);
    
    // Simulate encryption (empty payload with auth tag)
    generate_random(resp_msg->encrypted_nothing, 16);
    
    // Calculate MACs
    generate_random(resp_msg->mac1, 16);
    generate_random(resp_msg->mac2, 16);
    
    printf("Message Type: %d (Handshake Response)\n", resp_msg->type);
    printf("Sender Index: 0x%08X\n", ntohl(resp_msg->sender_index));
    printf("Receiver Index: 0x%08X\n", ntohl(resp_msg->receiver_index));
    print_hex("Ephemeral Public Key", resp_msg->ephemeral, 32);
    printf("\n");
    printf("Encrypted Empty: [16 bytes - auth tag only]\n");
    printf("MAC1: [16 bytes]\n");
    printf("MAC2: [16 bytes]\n");
    
    printf("\nCryptographic Operations:\n");
    printf("1. Verify MAC1 from initiation message\n");
    printf("2. Decrypt and verify initiator's static key\n");
    printf("3. Generate own ephemeral keypair\n");
    printf("4. Derive session keys using Noise protocol\n");
    printf("5. Encrypt empty payload (proves key derivation)\n");
    printf("6. Compute MAC1 over response\n");
    
    printf("\nTotal Message Size: %lu bytes\n", sizeof(wg_handshake_resp_t));
    printf("\n[✓] Handshake Complete! Session keys established.\n");
}

// Simulate data packet
void create_data_packet(wg_peer_t *sender, const char *payload, int payload_len) {
    printf("\n========== DATA PACKET ==========\n\n");
    printf("[%s] Sending data packet\n\n", sender->name);
    
    int total_size = sizeof(wg_data_packet_t) + payload_len + 16;
    wg_data_packet_t *pkt = (wg_data_packet_t *)malloc(total_size);
    
    pkt->type = WG_PACKET_DATA;
    memset(pkt->reserved, 0, 3);
    pkt->receiver_index = htonl(sender->session_index); // Would be peer's index
    pkt->counter = htobe64(sender->tx_counter++);
    
    // Simulate encryption
    generate_random(pkt->encrypted_data, payload_len + 16);
    
    printf("Message Type: %d (Transport Data)\n", pkt->type);
    printf("Receiver Index: 0x%08X\n", ntohl(pkt->receiver_index));
    printf("Counter: %lu\n", (unsigned long)be64toh(pkt->counter));
    printf("Payload Length: %d bytes (encrypted)\n", payload_len);
    printf("Auth Tag: 16 bytes (Poly1305)\n");
    printf("Total Overhead: 16 bytes (header + auth tag)\n");
    printf("\nActual Payload: \"%s\"\n", payload);
    printf("Encrypted Payload: [hidden - ChaCha20-Poly1305 encrypted]\n");
    
    printf("\nTotal Packet Size: %d bytes\n", total_size);
    
    free(pkt);
}

// Compare with IPsec
void compare_with_ipsec() {
    printf("\n\n========== WireGuard vs IPsec ==========\n\n");
    
    printf("WIREGUARD ADVANTAGES:\n");
    printf("---------------------\n");
    printf("✓ Simpler: ~4,000 lines of code vs ~400,000 for IPsec\n");
    printf("✓ Faster: Less cryptographic overhead, efficient implementation\n");
    printf("✓ Modern crypto: Curve25519, ChaCha20, Poly1305, BLAKE2\n");
    printf("✓ No cipher negotiation: One secure suite, period\n");
    printf("✓ Stealth: No response to unauthenticated packets\n");
    printf("✓ Roaming: Seamless endpoint changes (mobile friendly)\n");
    printf("✓ Lower latency: Simpler state machine\n");
    printf("✓ Built-in DoS protection: Cookie mechanism\n\n");
    
    printf("IPsec ADVANTAGES:\n");
    printf("-----------------\n");
    printf("✓ Mature: Decades of deployment and testing\n");
    printf("✓ Standardized: IETF standard, wide vendor support\n");
    printf("✓ Flexible: Many cipher suites, modes, options\n");
    printf("✓ Enterprise ready: Better key management infrastructure\n");
    printf("✓ Hardware offload: Widely supported in network equipment\n\n");
    
    printf("CODE COMPLEXITY COMPARISON:\n");
    printf("---------------------------\n");
    printf("WireGuard:     ~4,000 lines (Linux kernel implementation)\n");
    printf("IPsec:       ~400,000 lines (strongSwan)\n");
    printf("OpenVPN:     ~100,000 lines\n\n");
    
    printf("HANDSHAKE COMPARISON:\n");
    printf("---------------------\n");
    printf("WireGuard: 1-RTT (1 round trip)\n");
    printf("  Client → Server: Handshake Initiation (148 bytes)\n");
    printf("  Server → Client: Handshake Response (92 bytes)\n");
    printf("  Total: 2 messages, 240 bytes\n\n");
    
    printf("IPsec/IKEv2: 2-RTT (2 round trips)\n");
    printf("  IKE_SA_INIT: Exchange DH, nonces\n");
    printf("  IKE_AUTH: Authentication and create CHILD_SA\n");
    printf("  Total: 4+ messages, much larger\n\n");
    
    printf("CRYPTOGRAPHY:\n");
    printf("-------------\n");
    printf("WireGuard:\n");
    printf("  Key Exchange:    Curve25519 (ECDH)\n");
    printf("  Encryption:      ChaCha20\n");
    printf("  Authentication:  Poly1305\n");
    printf("  Hash:            BLAKE2s\n");
    printf("  Combined:        ChaCha20-Poly1305 AEAD\n\n");
    
    printf("IPsec (typical):\n");
    printf("  Key Exchange:    DH groups 14-20, ECDH\n");
    printf("  Encryption:      AES-CBC, AES-GCM, 3DES\n");
    printf("  Authentication:  HMAC-SHA256, HMAC-SHA512\n");
    printf("  Hash:            SHA-2 family\n");
    printf("  Many options:    Configuration complexity\n\n");
    
    printf("WHEN TO USE WIREGUARD:\n");
    printf("----------------------\n");
    printf("✓ Personal VPN\n");
    printf("✓ Site-to-site VPN (simple setups)\n");
    printf("✓ Cloud interconnects\n");
    printf("✓ Container networking\n");
    printf("✓ Mobile clients (roaming)\n");
    printf("✓ Performance-critical applications\n");
    printf("✓ Modern infrastructure\n\n");
    
    printf("WHEN TO USE IPsec:\n");
    printf("------------------\n");
    printf("✓ Enterprise deployments (established)\n");
    printf("✓ Compliance requirements (FIPS, etc.)\n");
    printf("✓ Hardware acceleration needed\n");
    printf("✓ Integration with existing IPsec infrastructure\n");
    printf("✓ Need specific cipher suites\n");
    printf("✓ Legacy system support\n");
}

int main() {
    srand(time(NULL));
    
    printf("WireGuard Protocol Simulation\n");
    printf("==============================\n");
    
    // Create peers
    wg_peer_t alice, bob;
    create_peer(&alice, "Alice");
    create_peer(&bob, "Bob");
    
    printf("\n[*] Created Peers:\n");
    printf("    Alice - Session Index: 0x%08X\n", alice.session_index);
    print_hex("    Alice Public Key", alice.public_key, 32);
    printf("\n    Bob - Session Index: 0x%08X\n", bob.session_index);
    print_hex("    Bob Public Key", bob.public_key, 32);
    
    // Simulate handshake
    wg_handshake_init_t init_msg;
    create_handshake_initiation(&alice, &bob, &init_msg);
    
    wg_handshake_resp_t resp_msg;
    create_handshake_response(&bob, &init_msg, &resp_msg);
    
    // Simulate data transfer
    create_data_packet(&alice, "Hello from Alice!", 17);
    create_data_packet(&bob, "Hi Alice! Message received.", 27);
    
    // Compare with IPsec
    compare_with_ipsec();
    
    return 0;
}

/*
 * COMPILATION:
 * gcc -o wireguard_sim 11_wireguard_concepts.c
 * 
 * RUN:
 * ./wireguard_sim
 * 
 * INTERVIEW QUESTIONS & ANSWERS:
 * 
 * Q1: What is WireGuard?
 * A: Modern VPN protocol designed for simplicity, speed, and security.
 *    - Uses state-of-the-art cryptography
 *    - Minimal codebase (~4,000 lines)
 *    - Built into Linux kernel (5.6+)
 *    - Faster than IPsec and OpenVPN
 *    - Created by Jason Donenfeld
 * 
 * Q2: What is the Noise Protocol Framework?
 * A: Cryptographic framework that WireGuard is based on.
 *    Specifically uses "Noise_IK" pattern:
 *    - I = Initiator knows responder's static public key
 *    - K = Responder knows initiator's static public key
 *    Provides 1-RTT handshake with perfect forward secrecy
 * 
 * Q3: Why is WireGuard faster than IPsec?
 * A: - Simpler code = less overhead
 *    - Modern ciphers (ChaCha20) optimized for software
 *    - No cipher negotiation
 *    - Efficient state machine
 *    - Better cache performance
 *    - Runs in kernel space
 * 
 * Q4: How does WireGuard handle roaming?
 * A: "Cryptokey Routing" - associates public keys with allowed IPs
 *    When packet arrives from new endpoint but correct key:
 *    - Automatically updates endpoint
 *    - No re-handshake needed
 *    - Seamless for mobile devices
 * 
 * Q5: What is the Cookie mechanism?
 * A: DoS protection:
 *    - Under load, server responds with cookie
 *    - Client must include cookie in MAC2
 *    - Proves client can receive at claimed IP
 *    - Similar to SYN cookies in TCP
 * 
 * Q6: How does key rotation work?
 * A: - Handshake every 2 minutes (if traffic)
 *    - Automatic, transparent
 *    - Old keys kept briefly for overlap
 *    - Provides perfect forward secrecy
 *    - No manual intervention needed
 * 
 * Q7: What about audit logging and monitoring?
 * A: Intentionally minimal:
 *    - No logs by design (privacy)
 *    - Can monitor via kernel netlink
 *    - Can track peer connections
 *    - Enterprise may need additional tooling
 * 
 * Q8: WireGuard vs OpenVPN?
 * A: WireGuard:
 *    - Faster (kernel space)
 *    - Simpler configuration
 *    - Better roaming
 *    - Modern crypto only
 *    - No cipher choice
 *    
 *    OpenVPN:
 *    - Userspace (easier to update)
 *    - More flexible
 *    - Better NAT traversal
 *    - More mature ecosystem
 *    - TCP mode available
 * 
 * Q9: Can WireGuard work through NAT?
 * A: Yes, very well:
 *    - Uses UDP (single port)
 *    - Keepalive packets maintain NAT mapping
 *    - Roaming handles endpoint changes
 *    - No special NAT-T needed
 * 
 * Q10: What are WireGuard's limitations?
 * A: - No dynamic IP assignment (use userspace tools)
 *    - No built-in authentication (pre-shared keys only)
 *    - Limited enterprise features
 *    - IP addresses must be configured
 *    - No multicast support
 *    - Young ecosystem (improving)
 * 
 * REAL-WORLD USAGE:
 * - Tailscale (mesh VPN using WireGuard)
 * - Mullvad VPN
 * - ProtonVPN
 * - CloudFlare WARP
 * - Many cloud providers
 * - Kubernetes CNI plugins
 * 
 * IMPLEMENTATION NOTES:
 * - Cross-platform: Linux, Windows, macOS, iOS, Android
 * - Kernel space: Linux, FreeBSD
 *  - Userspace: wireguard-go, wireguard-rs
 * - Library: embeddable-wg-library
 */
