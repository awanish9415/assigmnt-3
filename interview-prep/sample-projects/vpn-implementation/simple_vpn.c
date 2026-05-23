/*
 * Simple VPN Implementation (Educational Purpose)
 * 
 * This is a simplified VPN to demonstrate core concepts:
 * - TUN/TAP interface
 * - Packet encryption/decryption
 * - Tunneling
 * 
 * Note: This is for learning only, not production-ready!
 * Use WireGuard or IPsec for real deployments.
 * 
 * Compile:
 * gcc -o simple_vpn simple_vpn.c -lssl -lcrypto
 * 
 * Run:
 * sudo ./simple_vpn server 10.8.0.1
 * sudo ./simple_vpn client 10.8.0.2 SERVER_IP
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#define PORT 5555
#define BUFFER_SIZE 2048
#define KEY_SIZE 16  // AES-128

// Shared secret key (in real VPN, exchange via Diffie-Hellman)
unsigned char shared_key[KEY_SIZE] = "MySecretKey12345";

// Create TUN interface
int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;  // TUN mode, no packet info
    
    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

// Simple XOR encryption (demo only - use real crypto in production!)
void simple_encrypt(unsigned char *data, int len, unsigned char *key) {
    for (int i = 0; i < len; i++) {
        data[i] ^= key[i % KEY_SIZE];
    }
}

// Server mode
void run_server(const char *tun_ip) {
    char tun_name[IFNAMSIZ] = "tun0";
    int tun_fd, sock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    unsigned char buffer[BUFFER_SIZE];
    fd_set fds;

    // Create TUN interface
    if ((tun_fd = tun_alloc(tun_name)) < 0) {
        fprintf(stderr, "Error creating TUN interface\n");
        exit(1);
    }
    printf("[+] TUN interface %s created\n", tun_name);

    // Configure TUN interface
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ip addr add %s/24 dev %s", tun_ip, tun_name);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "ip link set %s up", tun_name);
    system(cmd);
    printf("[+] TUN interface configured with IP %s\n", tun_ip);

    // Create UDP socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    printf("[+] Listening on port %d\n", PORT);

    printf("[+] VPN server ready!\n\n");

    // Main loop
    while (1) {
        FD_ZERO(&fds);
        FD_SET(tun_fd, &fds);
        FD_SET(sock_fd, &fds);
        int max_fd = (tun_fd > sock_fd) ? tun_fd : sock_fd;

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            break;
        }

        // Data from TUN (to be sent to client)
        if (FD_ISSET(tun_fd, &fds)) {
            int len = read(tun_fd, buffer, BUFFER_SIZE);
            if (len < 0) {
                perror("Reading from TUN");
                continue;
            }
            
            printf("[TUN→NET] Read %d bytes from TUN\n", len);
            
            // Encrypt
            simple_encrypt(buffer, len, shared_key);
            
            // Send to client (if we have client address)
            // In production, maintain client address from handshake
            printf("[TUN→NET] Encrypted and ready to send\n");
        }

        // Data from network (from client)
        if (FD_ISSET(sock_fd, &fds)) {
            client_len = sizeof(client_addr);
            int len = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0,
                              (struct sockaddr*)&client_addr, &client_len);
            if (len < 0) {
                perror("Receiving from socket");
                continue;
            }
            
            printf("[NET→TUN] Received %d bytes from %s:%d\n",
                   len, inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));
            
            // Decrypt
            simple_encrypt(buffer, len, shared_key);
            
            // Write to TUN
            if (write(tun_fd, buffer, len) < 0) {
                perror("Writing to TUN");
                continue;
            }
            printf("[NET→TUN] Decrypted and written to TUN\n");
        }
    }

    close(tun_fd);
    close(sock_fd);
}

// Client mode
void run_client(const char *tun_ip, const char *server_ip) {
    char tun_name[IFNAMSIZ] = "tun0";
    int tun_fd, sock_fd;
    struct sockaddr_in server_addr;
    unsigned char buffer[BUFFER_SIZE];
    fd_set fds;

    // Create TUN interface
    if ((tun_fd = tun_alloc(tun_name)) < 0) {
        fprintf(stderr, "Error creating TUN interface\n");
        exit(1);
    }
    printf("[+] TUN interface %s created\n", tun_name);

    // Configure TUN interface
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ip addr add %s/24 dev %s", tun_ip, tun_name);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "ip link set %s up", tun_name);
    system(cmd);
    printf("[+] TUN interface configured with IP %s\n", tun_ip);

    // Create UDP socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid server IP\n");
        exit(1);
    }

    printf("[+] Connected to server %s:%d\n", server_ip, PORT);
    printf("[+] VPN client ready!\n\n");

    // Main loop
    while (1) {
        FD_ZERO(&fds);
        FD_SET(tun_fd, &fds);
        FD_SET(sock_fd, &fds);
        int max_fd = (tun_fd > sock_fd) ? tun_fd : sock_fd;

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            break;
        }

        // Data from TUN (to be sent to server)
        if (FD_ISSET(tun_fd, &fds)) {
            int len = read(tun_fd, buffer, BUFFER_SIZE);
            if (len < 0) {
                perror("Reading from TUN");
                continue;
            }
            
            printf("[TUN→NET] Read %d bytes from TUN\n", len);
            
            // Encrypt
            simple_encrypt(buffer, len, shared_key);
            
            // Send to server
            if (sendto(sock_fd, buffer, len, 0,
                      (struct sockaddr*)&server_addr,
                      sizeof(server_addr)) < 0) {
                perror("Sending to server");
                continue;
            }
            printf("[TUN→NET] Encrypted and sent to server\n");
        }

        // Data from network (from server)
        if (FD_ISSET(sock_fd, &fds)) {
            int len = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
            if (len < 0) {
                perror("Receiving from socket");
                continue;
            }
            
            printf("[NET→TUN] Received %d bytes from server\n", len);
            
            // Decrypt
            simple_encrypt(buffer, len, shared_key);
            
            // Write to TUN
            if (write(tun_fd, buffer, len) < 0) {
                perror("Writing to TUN");
                continue;
            }
            printf("[NET→TUN] Decrypted and written to TUN\n");
        }
    }

    close(tun_fd);
    close(sock_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  Server: %s server <tun_ip>\n", argv[0]);
        printf("  Client: %s client <tun_ip> <server_ip>\n", argv[0]);
        printf("\nExample:\n");
        printf("  Server: sudo %s server 10.8.0.1\n", argv[0]);
        printf("  Client: sudo %s client 10.8.0.2 SERVER_PUBLIC_IP\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "server") == 0) {
        run_server(argv[2]);
    } else if (strcmp(argv[1], "client") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Client mode requires server IP\n");
            return 1;
        }
        run_client(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Invalid mode. Use 'server' or 'client'\n");
        return 1;
    }

    return 0;
}

/*
 * TESTING:
 * 
 * Terminal 1 (Server):
 * sudo ./simple_vpn server 10.8.0.1
 * 
 * Terminal 2 (Client on same or different machine):
 * sudo ./simple_vpn client 10.8.0.2 127.0.0.1
 * 
 * Terminal 3 (Test - on client):
 * ping 10.8.0.1
 * 
 * INTERVIEW DISCUSSION POINTS:
 * - TUN vs TAP interfaces
 * - Why UDP for VPN transport?
 * - Encryption in real VPNs (AES-GCM, ChaCha20-Poly1305)
 * - Key exchange (Diffie-Hellman, IKE)
 * - Authentication and integrity
 * - Performance considerations
 * - MTU and fragmentation
 * - Routing and forwarding
 */
