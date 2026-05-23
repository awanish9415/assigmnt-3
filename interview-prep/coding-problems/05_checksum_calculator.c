/*
 * Problem 5: Network Checksum Calculator
 * 
 * Calculate and verify checksums for IP, TCP, UDP, and ICMP protocols.
 * Understanding checksums is crucial for protocol implementation.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Calculate Internet Checksum (RFC 1071)
uint16_t calculate_checksum(void *data, int length) {
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t *)data;
    
    // Sum all 16-bit words
    while (length > 1) {
        sum += *ptr++;
        length -= 2;
    }
    
    // Add remaining byte if odd length
    if (length > 0) {
        sum += *(uint8_t *)ptr;
    }
    
    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return (uint16_t)(~sum);
}

// TCP/UDP pseudo-header for checksum
typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t  zero;
    uint8_t  protocol;
    uint16_t length;
} pseudo_header_t;

// Calculate TCP/UDP checksum with pseudo-header
uint16_t calculate_tcp_udp_checksum(uint32_t src_ip, uint32_t dst_ip,
                                     uint8_t protocol, void *data, int length) {
    pseudo_header_t pseudo;
    pseudo.src_ip = src_ip;
    pseudo.dst_ip = dst_ip;
    pseudo.zero = 0;
    pseudo.protocol = protocol;
    pseudo.length = htons(length);
    
    uint32_t sum = 0;
    uint16_t *ptr;
    
    // Add pseudo-header
    ptr = (uint16_t *)&pseudo;
    for (int i = 0; i < sizeof(pseudo_header_t) / 2; i++) {
        sum += *ptr++;
    }
    
    // Add actual data
    ptr = (uint16_t *)data;
    int len = length;
    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    if (len > 0) {
        sum += *(uint8_t *)ptr;
    }
    
    // Fold and return
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return (uint16_t)(~sum);
}

int main() {
    // Test data
    char test_data[] = "Hello, Network Protocols!";
    
    printf("Network Checksum Calculator\n");
    printf("===========================\n\n");
    
    // IP checksum example
    printf("Test Data: \"%s\"\n", test_data);
    uint16_t checksum = calculate_checksum(test_data, strlen(test_data));
    printf("IP-style Checksum: 0x%04X\n\n", checksum);
    
    // TCP checksum example (with pseudo-header)
    uint32_t src_ip = 0x7F000001;  // 127.0.0.1
    uint32_t dst_ip = 0x08080808;  // 8.8.8.8
    checksum = calculate_tcp_udp_checksum(src_ip, dst_ip, 6, test_data, strlen(test_data));
    printf("TCP Checksum (with pseudo-header): 0x%04X\n", checksum);
    
    return 0;
}

/*
 * KEY POINTS:
 * - IP checksum covers header only
 * - TCP/UDP checksum includes pseudo-header (src IP, dst IP, protocol, length)
 * - ICMP checksum is like IP checksum but covers entire ICMP packet
 * - Checksum is one's complement of one's complement sum
 */
