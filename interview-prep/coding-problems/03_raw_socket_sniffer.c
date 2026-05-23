/*
 * Problem 3: Raw Socket Packet Capture (Packet Sniffer)
 * 
 * Description:
 * Create a simple packet sniffer using raw sockets to capture
 * network packets at Layer 2/3 level.
 * 
 * Key Concepts:
 * - Raw sockets (SOCK_RAW)
 * - Packet headers (Ethernet, IP, TCP, UDP)
 * - Network byte order
 * - Requires root/sudo privileges
 * 
 * Interview Topics:
 * - What are raw sockets?
 * - Security implications
 * - Promiscuous mode
 * - Packet structure
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

#define BUFFER_SIZE 65536

// Function to print Ethernet header
void print_ethernet_header(unsigned char *buffer) {
    struct ethhdr *eth = (struct ethhdr *)buffer;
    
    printf("\n");
    printf("Ethernet Header\n");
    printf("   |-Source MAC      : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",
           eth->h_source[0], eth->h_source[1], eth->h_source[2],
           eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    printf("   |-Destination MAC : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",
           eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
           eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    printf("   |-Protocol        : 0x%.4x\n", ntohs(eth->h_proto));
}

// Function to print IP header
void print_ip_header(unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in source, dest;
    
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
    
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;
    
    printf("\n");
    printf("IP Header\n");
    printf("   |-Version              : %d\n", (unsigned int)iph->version);
    printf("   |-Header Length        : %d DWORDS (%d Bytes)\n",
           (unsigned int)iph->ihl, ((unsigned int)(iph->ihl)) * 4);
    printf("   |-Type Of Service      : %d\n", (unsigned int)iph->tos);
    printf("   |-Total Length         : %d Bytes\n", ntohs(iph->tot_len));
    printf("   |-Identification       : %d\n", ntohs(iph->id));
    printf("   |-TTL                  : %d\n", (unsigned int)iph->ttl);
    printf("   |-Protocol             : %d\n", (unsigned int)iph->protocol);
    printf("   |-Checksum             : 0x%.4x\n", ntohs(iph->check));
    printf("   |-Source IP            : %s\n", inet_ntoa(source.sin_addr));
    printf("   |-Destination IP       : %s\n", inet_ntoa(dest.sin_addr));
}

// Function to print TCP header
void print_tcp_header(unsigned char *buffer) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    struct tcphdr *tcph = (struct tcphdr *)(buffer + iphdrlen + sizeof(struct ethhdr));
    
    printf("\n");
    printf("TCP Header\n");
    printf("   |-Source Port      : %u\n", ntohs(tcph->source));
    printf("   |-Destination Port : %u\n", ntohs(tcph->dest));
    printf("   |-Sequence Number  : %u\n", ntohl(tcph->seq));
    printf("   |-Acknowledge Number: %u\n", ntohl(tcph->ack_seq));
    printf("   |-Header Length    : %d DWORDS (%d Bytes)\n",
           (unsigned int)tcph->doff, (unsigned int)tcph->doff * 4);
    printf("   |-Flags            : ");
    if (tcph->urg) printf("URG ");
    if (tcph->ack) printf("ACK ");
    if (tcph->psh) printf("PSH ");
    if (tcph->rst) printf("RST ");
    if (tcph->syn) printf("SYN ");
    if (tcph->fin) printf("FIN ");
    printf("\n");
    printf("   |-Window Size      : %d\n", ntohs(tcph->window));
    printf("   |-Checksum         : 0x%.4x\n", ntohs(tcph->check));
    printf("   |-Urgent Pointer   : %d\n", tcph->urg_ptr);
}

// Function to print UDP header
void print_udp_header(unsigned char *buffer) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    struct udphdr *udph = (struct udphdr *)(buffer + iphdrlen + sizeof(struct ethhdr));
    
    printf("\n");
    printf("UDP Header\n");
    printf("   |-Source Port      : %d\n", ntohs(udph->source));
    printf("   |-Destination Port : %d\n", ntohs(udph->dest));
    printf("   |-UDP Length       : %d\n", ntohs(udph->len));
    printf("   |-UDP Checksum     : 0x%.4x\n", ntohs(udph->check));
}

// Function to print ICMP header
void print_icmp_header(unsigned char *buffer) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;
    struct icmphdr *icmph = (struct icmphdr *)(buffer + iphdrlen + sizeof(struct ethhdr));
    
    printf("\n");
    printf("ICMP Header\n");
    printf("   |-Type     : %d\n", (unsigned int)icmph->type);
    printf("   |-Code     : %d\n", (unsigned int)icmph->code);
    printf("   |-Checksum : 0x%.4x\n", ntohs(icmph->checksum));
}

// Process captured packet
void process_packet(unsigned char *buffer, int size) {
    static int packet_count = 0;
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    
    packet_count++;
    printf("\n\n===== Packet #%d =====\n", packet_count);
    printf("Packet Size: %d bytes\n", size);
    
    print_ethernet_header(buffer);
    print_ip_header(buffer, size);
    
    // Check protocol and print appropriate header
    switch (iph->protocol) {
        case IPPROTO_TCP:
            printf("\nProtocol: TCP\n");
            print_tcp_header(buffer);
            break;
            
        case IPPROTO_UDP:
            printf("\nProtocol: UDP\n");
            print_udp_header(buffer);
            break;
            
        case IPPROTO_ICMP:
            printf("\nProtocol: ICMP\n");
            print_icmp_header(buffer);
            break;
            
        default:
            printf("\nProtocol: Other (%d)\n", iph->protocol);
            break;
    }
    
    printf("======================================\n");
}

int main() {
    int sockfd;
    unsigned char *buffer;
    struct sockaddr saddr;
    int saddr_len, data_size;
    
    // Allocate buffer for packet
    buffer = (unsigned char *)malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    printf("Starting packet sniffer...\n");
    printf("Note: This program requires root/sudo privileges!\n\n");
    
    // Create raw socket
    // AF_PACKET: Capture packets at device driver level
    // SOCK_RAW: Raw socket
    // htons(ETH_P_ALL): Capture all ethernet protocols
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Socket creation failed. Are you running as root?");
        free(buffer);
        return 1;
    }
    
    printf("[+] Raw socket created successfully\n");
    printf("[+] Capturing packets... (Press Ctrl+C to stop)\n\n");
    
    // Capture packets
    while (1) {
        saddr_len = sizeof(saddr);
        
        // Receive packet
        data_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, &saddr, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("Packet receive failed");
            break;
        }
        
        // Process the packet
        process_packet(buffer, data_size);
    }
    
    close(sockfd);
    free(buffer);
    
    return 0;
}

/*
 * COMPILATION:
 * gcc -o packet_sniffer 03_raw_socket_sniffer.c
 * 
 * RUN (requires root):
 * sudo ./packet_sniffer
 * 
 * TEST:
 * While running, in another terminal:
 * - ping google.com (generates ICMP packets)
 * - curl http://example.com (generates TCP packets)
 * - dig google.com (generates UDP packets to DNS)
 * 
 * INTERVIEW QUESTIONS & ANSWERS:
 * 
 * Q1: What is a raw socket?
 * A: A raw socket allows direct access to underlying network protocols,
 *    bypassing the normal protocol processing. Can send/receive packets
 *    at IP layer or below (data link layer with AF_PACKET).
 * 
 * Q2: Why does this require root privileges?
 * A: Raw sockets can be used maliciously (spoofing, sniffing sensitive data).
 *    Only privileged users can create them for security reasons.
 * 
 * Q3: What is AF_PACKET vs AF_INET?
 * A: AF_PACKET: Captures at data link layer (Layer 2), includes Ethernet headers
 *    AF_INET: Works at network layer (Layer 3), starts from IP header
 * 
 * Q4: What is promiscuous mode?
 * A: Normally, NIC only accepts packets destined to it. In promiscuous mode,
 *    it accepts ALL packets on the network segment. Used for packet sniffing.
 *    Can be enabled with: setsockopt(sock, SOL_SOCKET, SO_PROMISC, ...)
 * 
 * Q5: How does Wireshark work internally?
 * A: Uses libpcap (on Unix) or WinPcap (Windows) which uses raw sockets
 *    and promiscuous mode to capture all network traffic. Then parses
 *    and displays protocol information.
 * 
 * Q6: What is ETH_P_ALL?
 * A: Protocol value to capture ALL ethernet frame types (IP, ARP, IPv6, etc.)
 *    Could use ETH_P_IP to capture only IPv4 packets.
 * 
 * Q7: Network byte order vs host byte order?
 * A: Network uses big-endian (MSB first)
 *    Host may use little-endian (Intel x86) or big-endian
 *    Functions: htons() (host to network short)
 *               htonl() (host to network long)
 *               ntohs() (network to host short)
 *               ntohl() (network to host long)
 * 
 * SECURITY IMPLICATIONS:
 * - Can capture passwords sent in plaintext
 * - Can analyze encrypted traffic patterns
 * - Used in penetration testing
 * - Can detect suspicious network activity
 * - Basis for IDS/IPS systems
 * 
 * REAL-WORLD APPLICATIONS:
 * - Network monitoring tools (Wireshark, tcpdump)
 * - Intrusion Detection Systems (Snort, Suricata)
 * - Network performance analysis
 * - Protocol development and debugging
 * - Security auditing
 * 
 * IMPROVEMENTS TO DISCUSS:
 * - Add packet filtering (BPF - Berkeley Packet Filter)
 * - Write packets to pcap file
 * - Add statistics (packets per protocol)
 * - Implement packet reassembly for fragmented packets
 * - Add deep packet inspection
 * - Parse application layer protocols (HTTP, DNS, etc.)
 */
