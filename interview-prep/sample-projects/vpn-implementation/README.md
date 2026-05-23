# Simple VPN Implementation

## Overview
Educational VPN implementation demonstrating core concepts of tunneling and encryption.

## Features
- TUN interface creation and management
- UDP-based transport
- Packet encryption/decryption (simplified)
- Client-Server architecture

## Build
```bash
gcc -o simple_vpn simple_vpn.c -lssl -lcrypto
```

## Usage

### Server
```bash
sudo ./simple_vpn server 10.8.0.1
```

### Client
```bash
sudo ./simple_vpn client 10.8.0.2 SERVER_IP
```

## Test
```bash
# On client
ping 10.8.0.1
```

## Interview Discussion Points
1. **TUN vs TAP**: Layer 3 vs Layer 2
2. **Transport Protocol**: Why UDP?
3. **Encryption**: Real-world algorithms
4. **Key Exchange**: Diffie-Hellman, IKE
5. **Performance**: Throughput, latency
6. **Security**: Authentication, integrity

## Limitations (By Design)
- Simple XOR encryption (use AES-GCM in production)
- No key exchange (hardcoded key)
- No authentication
- No session management
- Single client support

## Production Alternatives
- **WireGuard**: Modern, fast, simple
- **IPsec**: Enterprise standard
- **OpenVPN**: Mature, flexible
