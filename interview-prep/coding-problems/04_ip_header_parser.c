/*
 * Problem 4: IP Header Parser and Validator
 * 
 * Description:
 * Parse an IP packet header and validate its fields including checksum.
 * Understand the structure of IPv4 header in detail.
 * 
 * Key Concepts:
 * - IP header structure
 * - Checksum calculation
 * - Header options parsing
 * - Fragmentation fields
 * 
 * Interview Topics:
 * - IP header fields and their purpose
 * - How is checksum calculated?
 * - What is TTL and why is it needed?
 * - IP fragmentation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

// IPv4 Header Structure (20 bytes minimum)
typedef struct {
    uint8_t  version_ihl;      // Version (4 bits) + IHL (4 bits)
    uint8_t  tos;              // Type of Service
    uint16_t total_length;     // Total Length
    uint16_t identification;   // Identification
    uint16_t flags_fragment;   // Flags (3 bits) + Fragment Offset (13 bits)
    uint8_t  ttl;              // Time To Live
    uint8_t  protocol;         // Protocol
    uint16_t checksum;         // Header Checksum
    uint32_t source_ip;        // Source IP Address
    uint32_t dest_ip;          // Destination IP Address
    // Options and Padding (if IHL > 5)
} ip_header_t;

// Calculate IP header checksum
uint16_t calculate_checksum(uint16_t *data, int length) {
    uint32_t sum = 0;
    
    // Add all 16-bit words
    while (length > 1) {
        sum += *data++;
        length -= 2;
    }
    
    // Add remaining byte if odd length
    if (length > 0) {
        sum += *(uint8_t *)data;
    }
    
    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    // Return one's complement
    return (uint16_t)(~sum);
}

// Validate IP header checksum
int validate_checksum(ip_header_t *header) {
    uint16_t original_checksum = header->checksum;
    header->checksum = 0;
    
    uint16_t calculated = calculate_checksum((uint16_t *)header, sizeof(ip_header_t));
    header->checksum = original_checksum;
    
    return (calculated == original_checksum);
}

// Convert IP address to string
void ip_to_string(uint32_t ip, char *buffer) {
    sprintf(buffer, "%d.%d.%d.%d",
            (ip & 0xFF),
            ((ip >> 8) & 0xFF),
            ((ip >> 16) & 0xFF),
            ((ip >> 24) & 0xFF));
}

// Parse and display IP header
void parse_ip_header(uint8_t *packet, int packet_length) {
    ip_header_t *header = (ip_header_t *)packet;
    
    // Extract version and IHL
    uint8_t version = (header->version_ihl >> 4) & 0x0F;
    uint8_t ihl = header->version_ihl & 0x0F;
    uint8_t header_length = ihl * 4;  // IHL is in 32-bit words
    
    printf("\n========== IP HEADER ANALYSIS ==========\n\n");
    
    // Basic validation
    if (version != 4) {
        printf("[ERROR] Invalid IP version: %d (expected 4)\n", version);
        return;
    }
    
    if (ihl < 5) {
        printf("[ERROR] Invalid IHL: %d (minimum is 5)\n", ihl);
        return;
    }
    
    if (header_length > packet_length) {
        printf("[ERROR] Header length (%d) exceeds packet length (%d)\n",
               header_length, packet_length);
        return;
    }
    
    printf("[✓] Version: %d (IPv4)\n", version);
    printf("[✓] IHL: %d (Header Length: %d bytes)\n", ihl, header_length);
    
    // Type of Service (ToS)
    printf("\nType of Service: 0x%02X\n", header->tos);
    printf("   |-Precedence: %d\n", (header->tos >> 5) & 0x07);
    printf("   |-Delay: %s\n", (header->tos & 0x10) ? "Low" : "Normal");
    printf("   |-Throughput: %s\n", (header->tos & 0x08) ? "High" : "Normal");
    printf("   |-Reliability: %s\n", (header->tos & 0x04) ? "High" : "Normal");
    
    // Total Length
    uint16_t total_len = ntohs(header->total_length);
    printf("\nTotal Length: %d bytes\n", total_len);
    printf("Payload Length: %d bytes\n", total_len - header_length);
    
    // Identification
    printf("\nIdentification: 0x%04X (%d)\n",
           ntohs(header->identification), ntohs(header->identification));
    
    // Flags and Fragment Offset
    uint16_t flags_frag = ntohs(header->flags_fragment);
    uint8_t flags = (flags_frag >> 13) & 0x07;
    uint16_t fragment_offset = flags_frag & 0x1FFF;
    
    printf("\nFlags: 0x%X\n", flags);
    printf("   |-Reserved: %d\n", (flags >> 2) & 0x01);
    printf("   |-Don't Fragment (DF): %d\n", (flags >> 1) & 0x01);
    printf("   |-More Fragments (MF): %d\n", flags & 0x01);
    printf("Fragment Offset: %d (%d bytes)\n", fragment_offset, fragment_offset * 8);
    
    // TTL
    printf("\nTime To Live (TTL): %d\n", header->ttl);
    
    // Protocol
    printf("\nProtocol: %d ", header->protocol);
    switch (header->protocol) {
        case 1:  printf("(ICMP)\n"); break;
        case 6:  printf("(TCP)\n"); break;
        case 17: printf("(UDP)\n"); break;
        case 50: printf("(ESP - IPsec)\n"); break;
        case 51: printf("(AH - IPsec)\n"); break;
        default: printf("(Other)\n"); break;
    }
    
    // Checksum
    printf("\nHeader Checksum: 0x%04X\n", ntohs(header->checksum));
    if (validate_checksum(header)) {
        printf("[✓] Checksum is VALID\n");
    } else {
        printf("[✗] Checksum is INVALID\n");
    }
    
    // IP Addresses
    char src_ip[16], dst_ip[16];
    ip_to_string(header->source_ip, src_ip);
    ip_to_string(header->dest_ip, dst_ip);
    
    printf("\nSource IP: %s\n", src_ip);
    printf("Destination IP: %s\n", dst_ip);
    
    // Options (if present)
    if (ihl > 5) {
        int options_length = header_length - 20;
        printf("\nIP Options Present: %d bytes\n", options_length);
        printf("Options Data: ");
        uint8_t *options = packet + 20;
        for (int i = 0; i < options_length; i++) {
            printf("%02X ", options[i]);
        }
        printf("\n");
    }
    
    printf("\n========================================\n");
}

// Create a sample IP packet for testing
void create_sample_packet(uint8_t *packet) {
    ip_header_t *header = (ip_header_t *)packet;
    
    // Fill header fields
    header->version_ihl = 0x45;  // Version 4, IHL 5 (20 bytes)
    header->tos = 0x00;
    header->total_length = htons(60);  // 20 byte header + 40 byte payload
    header->identification = htons(12345);
    header->flags_fragment = htons(0x4000);  // DF flag set, no fragmentation
    header->ttl = 64;
    header->protocol = 6;  // TCP
    header->source_ip = 0x0100007F;  // 127.0.0.1 (localhost)
    header->dest_ip = 0x08080808;    // 8.8.8.8 (Google DNS)
    
    // Calculate and set checksum
    header->checksum = 0;
    header->checksum = calculate_checksum((uint16_t *)header, 20);
    
    // Add some dummy payload
    memset(packet + 20, 0xAB, 40);
}

int main(int argc, char *argv[]) {
    uint8_t packet[100];
    
    printf("IP Header Parser and Validator\n");
    printf("===============================\n");
    
    // Create sample packet
    create_sample_packet(packet);
    printf("\n[*] Created sample IP packet\n");
    
    // Parse and display
    parse_ip_header(packet, 60);
    
    // Test with corrupted checksum
    printf("\n\n[*] Testing with CORRUPTED checksum...\n");
    ip_header_t *header = (ip_header_t *)packet;
    header->checksum = htons(0x1234);  // Wrong checksum
    parse_ip_header(packet, 60);
    
    return 0;
}

/*
 * COMPILATION:
 * gcc -o ip_parser 04_ip_header_parser.c
 * 
 * RUN:
 * ./ip_parser
 * 
 * INTERVIEW QUESTIONS & ANSWERS:
 * 
 * Q1: What is IHL (Internet Header Length)?
 * A: IHL specifies the header length in 32-bit words. Minimum is 5 (20 bytes).
 *    Maximum is 15 (60 bytes) to accommodate options.
 *    Actual length = IHL × 4 bytes
 * 
 * Q2: How is IP checksum calculated?
 * A: 1. Set checksum field to 0
 *    2. Treat header as sequence of 16-bit words
 *    3. Add all words (with carries)
 *    4. Fold 32-bit result to 16 bits
 *    5. Take one's complement
 *    Checksum only covers header, not payload!
 * 
 * Q3: What is TTL and why is it needed?
 * A: Time To Live - decremented by 1 at each router. When it reaches 0,
 *    packet is dropped and ICMP Time Exceeded is sent back.
 *    Prevents infinite loops in case of routing errors.
 *    Initial values: Windows=128, Linux=64, Cisco=255
 * 
 * Q4: What is IP fragmentation?
 * A: When packet is too large for network's MTU (Maximum Transmission Unit),
 *    it's split into smaller fragments. Each fragment has:
 *    - Same identification number
 *    - MF (More Fragments) flag set (except last fragment)
 *    - Fragment offset indicating position
 *    Receiver reassembles using identification and offset.
 * 
 * Q5: What is the Don't Fragment (DF) flag?
 * A: When set, router must not fragment the packet. If packet is too large,
 *    router drops it and sends ICMP "Fragmentation Needed" message back.
 *    Used for Path MTU Discovery.
 * 
 * Q6: What protocols use which protocol numbers?
 * A: 1=ICMP, 6=TCP, 17=UDP, 41=IPv6, 47=GRE, 50=ESP, 51=AH, 89=OSPF
 * 
 * Q7: What is ToS (Type of Service) used for?
 * A: QoS (Quality of Service) to prioritize packets.
 *    Modern use: DSCP (Differentiated Services Code Point) uses first 6 bits.
 *    Last 2 bits: ECN (Explicit Congestion Notification)
 * 
 * Q8: Why does checksum only cover header?
 * A: IP layer doesn't guarantee data integrity - that's TCP/UDP's job.
 *    Header checksum only ensures routing information is correct.
 *    Faster processing at routers (don't need to read entire packet).
 * 
 * Q9: Maximum IP packet size?
 * A: 65,535 bytes (16-bit total length field)
 *    But typical MTU: Ethernet=1500, PPPoE=1492
 *    Jumbo frames can support up to 9000 bytes
 * 
 * REAL-WORLD SCENARIOS:
 * - Routers check TTL, decrement it, recalculate checksum
 * - Firewalls parse headers for filtering decisions
 * - Load balancers use IP addresses for routing
 * - IPsec uses protocol numbers to identify encrypted packets
 * - QoS systems use ToS field for traffic shaping
 * 
 * IMPROVEMENTS TO DISCUSS:
 * - Parse IP options (source routing, timestamp, etc.)
 * - Handle fragmented packets reassembly
 * - Support IPv6 header parsing
 * - Add packet modification capabilities
 * - Implement NAT translation logic
 * - Add GeoIP lookup for IP addresses
 */
