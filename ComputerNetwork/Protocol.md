# TCP/IP

## Concept

### OSI vs TCP/IP model

OSI: 7 layers (Physical → Application)

![image-20250921081103120](Protocol/image-20250921081103120.png)

TCP/IP: 4 layers (Link, Internet, Transport, Application)

![image-20250921081151515](Protocol/image-20250921081151515.png)

![image-20250921081206512](Protocol/image-20250921081206512.png)

### **IP**

Best-effort delivery, no guarantee of order/reliability.

```shell
#check networks interfaces and IPs
ip addr
#or
ifconfig
```

### **TCP**

Reliable, connection-oriented, ordered.

```shell
#TCP connections
netstat -tulnp | head
#or ss(Socket Statistics)
ss -tulnp | head
```

### **UDP**

Unreliable, connectionless, fast.

```shell
#use TCP and UDP set a connection
#TCP
nc -l 12345
#other terminal
nc 127.0.0.1 12345
```

use `lsof ` to check port. They are established.

![image-20250921084323263](Protocol/image-20250921084323263.png)

```shell
#UDP
nc -u -l 12346
#other terminal
nc -u 127.0.0.1 12346
```

not established.

![image-20250921084744873](Protocol/image-20250921084744873.png)

### **Ports**

Identify which process gets the data.

### **Encapsulation**

Data → Segment (TCP/UDP) → Packet (IP) → Frame (Ethernet).

## Q&A

### Why does TCP need a 3-way handshake instead of just 2?

Establish

![image-20250921085148679](Protocol/image-20250921085148679.png)

Close

![image-20250921085215916](Protocol/image-20250921085215916.png)

if just 2 way(SYN --> SYN-ACK), the server need allocate resources immediately after SYN-ACK.

But if client's SYN was a delayed/duplicated packet(from old connection). Then the resources are wasted.

The **third ACK from the client** proves that:

1. The client actually exists and is ready.
2. Synchronized `/ˈsɪŋ.krə.naɪzd/` : Both sides can reach each other in both directions.

### When choose UDP?

**Low latency, no overhead** → e.g. gaming, VoIP, video streaming.

**Application-level control** → you don’t want retransmissions or flow control (e.g. DNS queries, where retry logic is simpler at app layer).

**Broadcast/multicast support** → UDP can send to many receivers efficiently; TCP is point-to-point only.

UDP is chosen when speed and simplicity are more important than reliability.

### How does encapsulation help when designing modular networks?

Encapsulation makes networks modular, flexible, and scalable.

Encapsulation = each layer **wraps its data** with headers and passes it down:

- **Application → Transport → Network → Link → Physical**

- Example: An HTTP request (Application) gets encapsulated in TCP segments (Transport), then in IP packets (Network), then in Ethernet frames (Link).

Benefits:

1. **Separation of concerns** – each layer only needs to know how to talk to the layer directly above/below it, not the whole stack.
2. **Flexibility** – you can swap IP (IPv4 → IPv6), or swap Ethernet with Wi-Fi, without changing the application.
3. **Interoperability** – vendors can implement parts independently, as long as they respect interfaces.

# TCP

## Features

**Reliability Mechanisms**

- **Sequence numbers** → ensure ordered delivery and detect duplicates.
- **Acknowledgments (ACKs)** → receiver confirms what it has received.
- **Retransmission (RTO, fast retransmit)** → if ACK not received, resend.

**Flow Control (Receiver-Side)**

- Sliding window (advertised `rwnd`) → prevents sender from overwhelming receiver.
- Window size advertised in TCP header → sender must respect it.

*What happens if the receiver advertises window size = 0?* 

Sender pauses sending until receiver updates it.

**Congestion Control (Network-Side)**

- Classic algorithms: **Slow start, congestion avoidance, fast retransmit, fast recovery**.
- Goal: avoid congestive collapse of the Internet.
- Modern algorithms: **CUBIC, BBR**.

*Why does TCP start slow?* 

To probe available bandwidth without flooding.

**Connection Management**

- **Establishment** → 3-way handshake.
- **Teardown** → FIN → ACK → FIN → ACK (4-way close).
- **TIME_WAIT state** → prevents confusion from delayed packets.

**Other Features**

- **Nagle’s Algorithm** (reduce small packets; interacts badly with `delayed ACK`).
- **Delayed ACKs** (receiver waits briefly to ACK in case it can piggyback).
- **Keepalives** (to detect dead peers).
- **Out-of-order delivery handling**.

## Q&A

### Why does TCP need both flow and congestion control?

**Flow control** → protects the **receiver**. -- congestion window (cwnd)

**Congestion control** → protects the **network**.  

### What is fast retransmit vs timeout retransmit?

**Timeout retransmit**:

- Sender waits until **RTO (Retransmission Timeout)** expires.
- Only then resends the packet.
- Problem → slow recovery when a packet is lost.

**Fast retransmit**:

- Triggered by **3 duplicate ACKs** (receiver keeps ACKing the last in-order packet).
- Sender infers that a segment was lost (but later packets arrived).
- Retransmits **immediately**, without waiting for RTO.

Fast retransmit is quicker and avoids waiting for timeout; timeout retransmit is the fallback when ACKs don’t arrive at all.

### Why is TCP fair between flows?

- Congestion control algorithms

  - Each flow slowly increases cwnd (additive increase).

  - When loss occurs, all affected flows reduce cwnd (multiplicative decrease).

- Over time → multiple flows sharing a link tend to get a roughly equal share of bandwidth.

TCP fairness comes from the fact that all flows back off when there’s congestion, so no single flow hogs the link forever.

*TCP achieves reliability and fairness with a mix of flow control (receiver safety), congestion control (network safety), and retransmission strategies (fast recovery). That’s why TCP flows usually share bandwidth fairly.*

# UDP & QUIC

## UDP

**Connectionless**: No handshake, just fire-and-forget.

**Unreliable**: No guarantees of delivery, order, or duplication.

**Lightweight**: Small header (8 bytes).

Used in: DNS, VoIP, video streaming, online games.

*Why use UDP when TCP is reliable?*

**Lower latency** → no 3-way handshake.

**Custom reliability** → app can add its own retransmission (like QUIC, RTP).

**Broadcast/multicast support**.

## QUIC

Runs **on top of UDP** (user-space protocol).

Designed by Google, now IETF standard.

Key features:

1. **Built-in TLS 1.3 encryption** (no need for separate TLS handshake).
2. **Multiplexed streams** → avoid TCP *head-of-line blocking*.
3. **Faster connection setup** → 1-RTT, sometimes 0-RTT.
4. **Better mobility** → connection survives IP change (Wi-Fi ↔ 4G).

*Why QUIC over TCP+TLS+HTTP/2?*

QUIC integrates transport + crypto + multiplexing → lower latency and better resilience.



**HTTP/1.0 & 1.1** → over **TCP**

**HTTP/2** → over **TCP** (with TLS usually)

**HTTP/3** → over **QUIC** (and QUIC itself runs over **UDP**)

# IP & Routing

## concept

### **IP basic**

- IP = Internet Protocol, provides **logical addressing** and **best-effort delivery**.
- Two versions: 
  - **IPv4 (32-bit addresses)**   
    -  (~4.3 billion) dotted decimal (e.g., `192.168.1.1`)
  - **IPv6 (128-bit addresses)**.
    - (~3.4×10^38), hexadecimal, separated by colons (e.g., `2001:0db8::1`).

### **Subnetting**

Divides a network into smaller **sub-networks**.

Helps:

1. Efficient IP use.
2. Localizes traffic → reduces broadcast domains.

Example: `192.168.1.0/24` → 256 addresses; `/26` → 64 addresses per subnet.

### **Routing Tables**

- Routers use them to decide **where to forward packets**.
- Each entry = `(destination network, subnet mask, next hop, interface)`.
- **Longest prefix match** is used to pick the most specific route.
- Default route (`0.0.0.0/0`) → catch-all when no specific route exists.

### **NAT**

NAT = **Network Address Translation**

- Maps **private IPs** to **public IP** for Internet access.
- Types: static, dynamic, PAT (overload).
- Solves **IPv4 address shortage**, but breaks end-to-end connectivity.

### **ICMP **

ICMP = Internet Control Message Protocol

- Used for **network diagnostics and error messages**.
- Examples:
  - `ping` → Echo Request / Reply.
  - `traceroute` → Time Exceeded messages.
- **Not for data transfer**; just control/info.

### ARP 

ARP = **Address Resolution Protocol**

- Maps **IP addresses → MAC addresses** in a LAN.
- Example: if `192.168.1.100` wants to send a packet, it asks “Who has this IP?” → gets MAC → can send Ethernet frame.
- IPv6 uses **Neighbor Discovery Protocol (NDP)** instead of ARP.

**Routing**

- Routers forward packets based on **destination IP**.
- Uses **routing tables** and algorithms (RIP, OSPF, BGP).
- Packet forwarding = **longest prefix match**.

**Fragmentation**

- If a packet is bigger than the link MTU(Max trans Unit), IPv4 may fragment it.
- IPv6 avoids this by requiring end-hosts to handle MTU discovery.

## Q&A

### Why do we need **logical IP addresses** instead of just MAC addresses?

**MAC address**: tied to the hardware,works only in the **local network (LAN)**. Switches use it.

**Problem**: MACs don’t provide hierarchy. You can’t “aggregate” routes — the internet would need billions of entries.

**IP address**: logical, hierarchical (network + host). Makes routing scalable, because routers only need prefixes(e.g. `192.168.0.0/16`).

Without IP, the global internet routing would be impossible.

### What happens if a router doesn’t know the route to a destination?

Router checks its **routing table**

If no entry →

- If there’s a **default route (0.0.0.0/0)**, it forwards the packet there.
- If no default → router **drops the packet** and usually sends back an **ICMP “Destination Unreachable”**.

### Compare **IPv4 NAT** vs **IPv6 global addressing**: which do you think is better for scalability?

**IPv4 + NAT**:

- Pros: solves address exhaustion, lets many devices share one public IP.
- Cons: breaks end-to-end connectivity, complicates peer-to-peer, VoIP, some protocols need NAT traversal (STUN, TURN, etc.).

**IPv6 global addressing**:

- Pros: each device gets a unique address, restores clean end-to-end model, simplifies routing.
- Cons: deployment is still slower, many networks still rely on IPv4.

**For scalability**: **IPv6 is better** (huge address space, no NAT hacks).
 But in practice, IPv4 + NAT is still dominant due to compatibility.

### How to calculate subnet mask, network ID, and host range？

[IP地址计算---子网掩码确定和子网划分等详解 (附常见相关习题)_子网掩码计算例题和讲解-CSDN博客](https://blog.csdn.net/qq_41207757/article/details/107839099)

# HTTP/HTTPS & APP Layer

## Concept

**HTTP (HyperText Transfer Protocol)**

- Protocol for **request/response communication** between client (browser) and server.
- **Stateless**: each request is independent.
- Common methods: `GET`, `POST`, `PUT`, `DELETE`, `HEAD`.

**HTTPS**

- HTTP over **TLS/SSL** → encrypts communication. (Use TLS handshake) 	`curl -v https://example.com`
- Prevents eavesdropping, tampering, and impersonation.
- Port 443 (HTTPS) vs port 80 (HTTP).

**Application Layer**

- Top layer in the TCP/IP model.
- Responsible for **network services** like:
  - Web (HTTP/HTTPS)
  - Email (SMTP, IMAP, POP3)
  - File transfer (FTP, SFTP)
  - DNS (domain name resolution)
- Provides **interfaces for user applications** to communicate over network.

**Request/Response Flow**

- Client sends **HTTP request** → Server processes → Sends **HTTP response**.
- Response contains **status code**, headers, and optional body.
- Example: `HTTP/1.1 200 OK`, `Content-Type: text/html`, body = HTML content.

## Q&A

### Why is HTTP stateless? How do websites maintain sessions?

**HTTP is stateless** because:

- Each request/response is independent.
- Server doesn’t store any information about previous requests.

**Benefits:**

- Simplicity: server doesn’t need to remember clients.
- Scalability: easy to handle many clients without session memory

**Maintaining sessions:**

Websites need to **remember users** (login, cart, preferences). 

Common mechanisms:

1. **Cookies** – small data stored on client, sent with each request.
   - Example: `Set-Cookie: session_id=abc123`
2. **Session IDs on server** – server maps session_id → user data.
3. **Tokens (JWT)** – client stores token; server verifies it.

### Why HTTPS is critical for sensitive data (login, payment)?

HTTPS = HTTP + TLS/SSL → **encrypts communication**.

**Reasons it’s essential:**

- **Confidentiality:** prevents eavesdroppers from reading passwords, credit card info.
- **Integrity:** prevents tampering of data in transit.
- **Authentication:** server proves its identity (certificates) → prevents MITM attacks.

Without HTTPS, login credentials, payments, and personal data can be **intercepted** or **modified**.

### What is the difference between HTTP/1.1, HTTP/2, and HTTP/3?

| Feature                | HTTP/1.1                                | HTTP/2                                     | HTTP/3                                                       |
| ---------------------- | --------------------------------------- | ------------------------------------------ | ------------------------------------------------------------ |
| **Transport**          | TCP                                     | TCP                                        | QUIC (UDP-based)                                             |
| **Multiplexing**       | No → 1 connection = 1 request at a time | Yes → multiple streams over one connection | Yes → multiple streams + 0-RTT connection                    |
| **Header compression** | No → repeated headers                   | Yes → HPACK                                | Yes → QPACK                                                  |
| **Latency**            | Higher due to multiple TCP connections  | Lower, pipelining & multiplexing           | Even lower, avoids TCP handshake/retransmit head-of-line blocking |
| **Security**           | Optional (TLS separately)               | Usually via TLS                            | Built-in (QUIC + TLS)                                        |

**0-RTT** (Zero Round-Trip Time) is a **feature of QUIC and TLS 1.3** that lets a client start sending data **immediately on a new connection** without waiting for a full handshake to complete.

HTTP/1.1 → simple, stateless, one request at a time → slower.

HTTP/2 → multiplexing, header compression → faster.

HTTP/3 → QUIC/UDP → avoids TCP head-of-line blocking, faster in lossy networks.