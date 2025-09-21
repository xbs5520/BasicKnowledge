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
2. Synchronized: Both sides can reach each other in both directions.

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