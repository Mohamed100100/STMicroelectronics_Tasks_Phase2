# Wirefish - A Packet Sniffer in C (Object-Oriented Design)

## 📌 Introduction
**Wirefish** is a packet sniffer written in **C**, following **Object-Oriented Programming (OOP) principles**. The project is designed to analyze network traffic, detecting and classifying different protocols at various layers of the **OSI model**. It supports key **Application, Transport, and Network Layer** protocols and provides structured data for each captured packet.

---

## 📚 Supported Protocols & OSI Model
Wirefish operates at multiple layers of the **OSI Model**:

| **OSI Layer**       | **Supported Protocols** |
|---------------------|------------------------|
| **Data Link Layer** | Ethernet, Loopback (lo) |
| **Network Layer**   | IP                      |
| **Transport Layer** | TCP, UDP, ICMP         |
| **Application Layer** | HTTP, FTP, DNS       |

### **Supported Protocols & Hierarchy**
- **IP** - The base protocol for network communication.
- **TCP, UDP, ICMP** - Transport layer protocols inheriting from IP.
- **DNS** - Inherits from UDP.
- **HTTP, FTP** - Inherit from TCP.

Each detected packet type contains the necessary encapsulated data:
- **HTTP** (Includes TCP & IP Data)
- **FTP** (Includes TCP & IP Data)
- **DNS** (Includes UDP & IP Data)
- **TCP** (Includes IP Data)
- **UDP** (Includes IP Data)
- **ICMP** (Includes IP Data)
- **IP** (Raw IP Data)

---
![Project Flow](images/Project_Flow.png)

---

## 🎨 **Class Hierarchy (OOP in C)**

Below is the **class hierarchy** for Wirefish, showcasing **inheritance relationships**:

```plaintext
        ┌─────────┐
        │   IP    │
        └─────────┘
             ▲
 ┌───────────┴───────────┐
 │          │            │
 │         UDP          TCP
 │          │            │
 │         DNS         HTTP, FTP
 │
ICMP
```

**Explanation:**
- **IP** is the base class for all network layer packets.
- **TCP, UDP, ICMP** inherit from IP.
- **DNS** inherits from UDP.
- **HTTP, FTP** inherit from TCP.

---
![Class Diagram](images/class.png)
---

## 🚀 Features
✔️ **Packet Sniffing**: Captures and analyzes network packets in real-time.  
✔️ **Protocol Detection**: Classifies packets into HTTP, FTP, DNS, TCP, UDP, ICMP, and IP.  
✔️ **OOP in C**: Implements an object-oriented approach using structs and function pointers.  
✔️ **Filtering**: Supports IP and port-based filtering.  


---

## 🛠️ Installation & Usage
### **1. Install Dependencies**
```sh
sudo apt install libpcap
```

### **2. Build the Project**
```sh
gcc -o wirefish sniffer.c IPclass.c TCPclass.c UDPclass.c FTPclass.c HTTPclass.c DNSclass.c ICMPclass.c -lpcap
```
### **3. Run the Packet Sniffer**
```sh
sudo ./wirefish <interface> [-ip <IP>] [-port <PORT>]
```
### **Example**
```sh
 sudo ./wirefish enp0s25 -ip 8.8.8.8 -port 80
```


## Dynamic Filtering (Ctrl+C Feature)

### When **Ctrl+C** is pressed, the program pauses, allowing modification of filters(IP,Port,Interface).
```sh
==================================
========= Sniffer Paused =========
==================================
Current Filters:
Interface: enp0s25
IP Filter: 8.8.8.8
Port Filter: None

Options:
1) Edit IP filter
2) Edit Port filter
3) Edit Interface
4) Reset filters
5) Resume sniffing
6) Exit

Enter choice:
```

---





