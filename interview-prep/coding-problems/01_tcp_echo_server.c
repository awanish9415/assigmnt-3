/*
 * Problem 1: TCP Echo Server
 * 
 * Description:
 * Create a TCP server that accepts client connections and echoes back
 * whatever data it receives from the client.
 * 
 * Key Concepts:
 * - Socket creation
 * - Binding to port
 * - Listening for connections
 * - Accepting connections
 * - recv/send operations
 * 
 * Interview Topics:
 * - Explain TCP 3-way handshake
 * - What happens during bind()?
 * - Difference between listen() and accept()
 * - How to handle multiple clients?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_PENDING 5

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    // Step 1: Create socket
    // AF_INET = IPv4, SOCK_STREAM = TCP
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("[+] Socket created successfully\n");
    
    // Step 2: Set socket options (optional but recommended)
    // SO_REUSEADDR allows reusing the port immediately after program exits
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    // Step 3: Setup server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;          // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    server_addr.sin_port = htons(PORT);        // Convert to network byte order
    
    // Step 4: Bind socket to address and port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("[+] Bound to port %d\n", PORT);
    
    // Step 5: Listen for incoming connections
    // MAX_PENDING is the backlog - max queued connections
    if (listen(server_fd, MAX_PENDING) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("[+] Listening for connections...\n");
    
    // Step 6: Accept and handle connections (simple single-client version)
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("[+] Client connected from %s:%d\n", 
           inet_ntoa(client_addr.sin_addr), 
           ntohs(client_addr.sin_port));
    
    // Step 7: Echo loop - receive and send back
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received < 0) {
            perror("Receive failed");
            break;
        } else if (bytes_received == 0) {
            printf("[+] Client disconnected\n");
            break;
        }
        
        printf("[+] Received: %s", buffer);
        
        // Echo back to client
        if (send(client_fd, buffer, bytes_received, 0) < 0) {
            perror("Send failed");
            break;
        }
    }
    
    // Cleanup
    close(client_fd);
    close(server_fd);
    printf("[+] Server closed\n");
    
    return 0;
}

/*
 * COMPILATION:
 * gcc -o tcp_echo_server 01_tcp_echo_server.c
 * 
 * RUN:
 * ./tcp_echo_server
 * 
 * TEST:
 * In another terminal: telnet localhost 8080
 * Or use: nc localhost 8080
 * 
 * INTERVIEW QUESTIONS & ANSWERS:
 * 
 * Q1: What is the difference between listen() and accept()?
 * A: listen() marks the socket as passive (ready to accept connections) and 
 *    sets the queue size for pending connections. accept() actually accepts
 *    a connection from the queue and returns a new socket for communication.
 * 
 * Q2: Why do we need SO_REUSEADDR?
 * A: When a TCP socket is closed, it enters TIME_WAIT state for 2*MSL (typically
 *    60-120 seconds). SO_REUSEADDR allows binding to the same port immediately,
 *    useful during development/testing.
 * 
 * Q3: What is the backlog parameter in listen()?
 * A: It's the maximum number of pending connections in the queue. If the queue
 *    is full and a new connection arrives, it may be refused.
 * 
 * Q4: How would you handle multiple clients?
 * A: Options include:
 *    - Fork a new process for each client (process-based)
 *    - Create a thread for each client (thread-based)
 *    - Use select/poll/epoll for I/O multiplexing (event-driven)
 *    - Use async I/O
 * 
 * Q5: What is INADDR_ANY?
 * A: It's 0.0.0.0, means accept connections on all network interfaces
 *    (localhost, ethernet, wifi, etc.)
 * 
 * Q6: Why use htons() for port?
 * A: Network byte order is big-endian. Most CPUs use little-endian.
 *    htons() (host to network short) converts port number to network byte order.
 * 
 * IMPROVEMENTS TO DISCUSS:
 * - Add timeout handling
 * - Implement graceful shutdown (handle SIGINT)
 * - Add connection limiting
 * - Implement keep-alive
 * - Add logging with timestamps
 * - Handle partial sends (send may not send all data)
 */
