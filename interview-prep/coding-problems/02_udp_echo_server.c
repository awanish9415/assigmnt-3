/*
 * Problem 2: UDP Echo Server and Client
 * 
 * Description:
 * Create a UDP server that receives datagrams and echoes them back.
 * Also create a simple UDP client to test it.
 * 
 * Key Concepts:
 * - UDP socket creation (SOCK_DGRAM)
 * - recvfrom/sendto operations
 * - Connectionless communication
 * - No connection state management
 * 
 * Interview Topics:
 * - TCP vs UDP differences
 * - When to use UDP?
 * - UDP reliability issues
 * - UDP packet size limitations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

// ==================== UDP ECHO SERVER ====================

void run_udp_server() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    // Create UDP socket (SOCK_DGRAM)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] UDP Socket created\n");
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Bound to port %d\n", PORT);
    printf("[SERVER] Waiting for datagrams...\n");
    
    // No listen() or accept() for UDP - it's connectionless!
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        client_len = sizeof(client_addr);
        
        // recvfrom() receives data and stores client address
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes_received < 0) {
            perror("Receive failed");
            continue;
        }
        
        printf("[SERVER] Received from %s:%d: %s",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);
        
        // Echo back to the same client
        if (sendto(sockfd, buffer, bytes_received, 0,
                   (struct sockaddr*)&client_addr, client_len) < 0) {
            perror("Send failed");
        } else {
            printf("[SERVER] Echoed back to client\n");
        }
    }
    
    close(sockfd);
}

// ==================== UDP ECHO CLIENT ====================

void run_udp_client(const char* server_ip, const char* message) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    socklen_t server_len;
    
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("[CLIENT] UDP Socket created\n");
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Send message to server
    printf("[CLIENT] Sending: %s\n", message);
    if (sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Receive echo response
    memset(buffer, 0, BUFFER_SIZE);
    server_len = sizeof(server_addr);
    
    // Set timeout for receive (optional but recommended)
    struct timeval tv;
    tv.tv_sec = 5;  // 5 seconds timeout
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                              (struct sockaddr*)&server_addr, &server_len);
    
    if (bytes_received < 0) {
        perror("Receive failed or timeout");
    } else {
        printf("[CLIENT] Received echo: %s\n", buffer);
    }
    
    close(sockfd);
}

// ==================== MAIN ====================

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // Run as server
        printf("Running in SERVER mode\n");
        run_udp_server();
    } else if (argc == 3) {
        // Run as client: ./program <server_ip> <message>
        printf("Running in CLIENT mode\n");
        run_udp_client(argv[1], argv[2]);
    } else {
        printf("Usage:\n");
        printf("  Server mode: %s\n", argv[0]);
        printf("  Client mode: %s <server_ip> <message>\n", argv[0]);
        return 1;
    }
    
    return 0;
}

/*
 * COMPILATION:
 * gcc -o udp_echo 02_udp_echo_server.c
 * 
 * RUN SERVER:
 * ./udp_echo
 * 
 * RUN CLIENT (in another terminal):
 * ./udp_echo 127.0.0.1 "Hello UDP!"
 * 
 * INTERVIEW QUESTIONS & ANSWERS:
 * 
 * Q1: Key differences between TCP and UDP?
 * A: TCP:
 *    - Connection-oriented (3-way handshake)
 *    - Reliable (ACKs, retransmission)
 *    - Ordered delivery
 *    - Flow control, congestion control
 *    - Higher overhead
 *    UDP:
 *    - Connectionless
 *    - Unreliable (no ACKs)
 *    - No guaranteed order
 *    - No flow/congestion control
 *    - Lower overhead, faster
 * 
 * Q2: When would you use UDP over TCP?
 * A: Use UDP when:
 *    - Speed is more important than reliability (gaming, video streaming)
 *    - Small, independent messages (DNS queries)
 *    - Broadcast/multicast needed
 *    - Custom reliability can be implemented at application layer
 *    - Real-time applications where old data is useless
 * 
 * Q3: Why don't we need listen() and accept() for UDP?
 * A: UDP is connectionless. There's no connection establishment phase.
 *    The socket is ready to receive data immediately after bind().
 *    Each datagram is independent and self-contained.
 * 
 * Q4: What is the maximum UDP packet size?
 * A: Theoretical max: 65,535 bytes (16-bit length field in IP header)
 *    Practical max: Usually 65,507 bytes (65,535 - 8 byte UDP header - 20 byte IP header)
 *    Safe max: 508-548 bytes to avoid fragmentation on most networks
 *    MTU consideration: Ethernet MTU is typically 1500 bytes
 * 
 * Q5: How does recvfrom() differ from recv()?
 * A: recvfrom() is used for UDP and also returns the sender's address.
 *    recv() is used for TCP where connection is already established.
 * 
 * Q6: What happens if UDP packet is lost?
 * A: Nothing! UDP doesn't detect or recover from packet loss.
 *    Application must implement its own reliability if needed.
 * 
 * Q7: Can UDP packets arrive out of order?
 * A: Yes! There's no sequencing in UDP. Application must handle ordering
 *    if needed (e.g., add sequence numbers).
 * 
 * REAL-WORLD UDP PROTOCOLS:
 * - DNS (Domain Name System)
 * - DHCP (Dynamic Host Configuration Protocol)
 * - SNMP (Simple Network Management Protocol)
 * - RTP (Real-time Transport Protocol) for VoIP
 * - QUIC (Quick UDP Internet Connections) - modern alternative to TCP
 * - Gaming protocols
 * - Live streaming
 * 
 * IMPROVEMENTS TO DISCUSS:
 * - Add checksum verification
 * - Implement reliability layer (ACKs, retransmission)
 * - Add sequence numbers for ordering
 * - Implement sliding window for flow control
 * - Handle packet fragmentation
 * - Add encryption (DTLS - Datagram TLS)
 */
