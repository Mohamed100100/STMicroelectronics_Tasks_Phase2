#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>


#include <netinet/ip.h>
#include <netinet/in.h>
#include <pcap.h>

#include "TCPclass.h"

void tcpclass_t::PrintSourcePort() {
    printf("Source Port: %u\n", ntohs(this->source));
}

void tcpclass_t::PrintDestPort() {
    printf("Destination Port: %u\n", ntohs(this->dest));
}

void tcpclass_t::PrintSeqNum() {
    printf("Sequence Number: %u\n", ntohl(this->seq));
}

void tcpclass_t::PrintAckNum() {
    printf("Acknowledgment Number: %u\n", ntohl(this->ack_seq));
}

void tcpclass_t::PrintOffset() {
    printf("Data Offset (Header Length): %u (bytes)\n", this->offset * 4);
}

void tcpclass_t::PrintReserved() {
    printf("Reserved Bits: %u\n", this->reserved);
}

// Print Individual Flags
void tcpclass_t::PrintURG() {
    printf("URG Flag: %s\n", this->urg ? "Set" : "Not Set");
}

void tcpclass_t::PrintACK() {
    printf("ACK Flag: %s\n", this->ack ? "Set" : "Not Set");
}

void tcpclass_t::PrintPSH() {
    printf("PSH Flag: %s\n", this->psh ? "Set" : "Not Set");
}

void tcpclass_t::PrintRST() {
    printf("RST Flag: %s\n", this->rst ? "Set" : "Not Set");
}

void tcpclass_t::PrintSYN() {
    printf("SYN Flag: %s\n", this->syn ? "Set" : "Not Set");
}

void tcpclass_t::PrintFIN() {
    printf("FIN Flag: %s\n", this->fin ? "Set" : "Not Set");
}

void tcpclass_t::PrintWindowSize() {
    printf("Window Size: %u\n", ntohs(this->window));
}

void tcpclass_t::PrintChecksum() {
    printf("Checksum: 0x%04X\n", ntohs(this->check));
}

void tcpclass_t::PrintUrgPtr() {
    printf("Urgent Pointer: %u\n", ntohs(this->urg_ptr));
}

// Print All TCP Data
void tcpclass_t::PrintAllTCPDetails() {
    this->PrintSourcePort();
    this->PrintDestPort();
    this->PrintSeqNum();
    this->PrintAckNum();
    this->PrintOffset();
    this->PrintReserved();
    this->PrintURG();
    this->PrintACK();
    this->PrintPSH();
    this->PrintRST();
    this->PrintSYN();
    this->PrintFIN();
    this->PrintWindowSize();
    this->PrintChecksum();
    this->PrintUrgPtr();
}

tcpclass_t::tcpclass_t(){}
tcpclass_t::tcpclass_t(const  unsigned char *packet):ipclass_t(packet)
{
    struct iphdr *ip_header = (struct iphdr *)(packet + 14); 
    struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + (ip_header->ihl)*4);  

    this->source = tcp_header->source;
    this->dest = tcp_header->dest;
    this->seq = tcp_header->seq;
    this->ack_seq = tcp_header->ack_seq;
    
    this->offset = tcp_header->doff;

    this->reserved = tcp_header->res1;
    this->urg = tcp_header->urg;
    this->ack = tcp_header->ack;
    this->psh = tcp_header->psh;
    this->rst = tcp_header->rst;
    this->syn = tcp_header->syn;
    this->fin = tcp_header->fin;

    this->window = tcp_header->window;
    this->check = tcp_header->check;
    this->urg_ptr = tcp_header->urg_ptr;
}

