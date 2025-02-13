#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>


#include <netinet/ip.h>
#include <netinet/in.h>
#include <pcap.h>

#include "TCPclass.h"

void tcp_PrintSourcePort(tcpclass_t *tcpObj) {
    printf("Source Port: %u\n", ntohs(tcpObj->source));
}

void tcp_PrintDestPort(tcpclass_t *tcpObj) {
    printf("Destination Port: %u\n", ntohs(tcpObj->dest));
}

void tcp_PrintSeqNum(tcpclass_t *tcpObj) {
    printf("Sequence Number: %u\n", ntohl(tcpObj->seq));
}

void tcp_PrintAckNum(tcpclass_t *tcpObj) {
    printf("Acknowledgment Number: %u\n", ntohl(tcpObj->ack_seq));
}

void tcp_PrintOffset(tcpclass_t *tcpObj) {
    printf("Data Offset (Header Length): %u (bytes)\n", tcpObj->offset * 4);
}

void tcp_PrintReserved(tcpclass_t *tcpObj) {
    printf("Reserved Bits: %u\n", tcpObj->reserved);
}

// Print Individual Flags
void tcp_PrintURG(tcpclass_t *tcpObj) {
    printf("URG Flag: %s\n", tcpObj->urg ? "Set" : "Not Set");
}

void tcp_PrintACK(tcpclass_t *tcpObj) {
    printf("ACK Flag: %s\n", tcpObj->ack ? "Set" : "Not Set");
}

void tcp_PrintPSH(tcpclass_t *tcpObj) {
    printf("PSH Flag: %s\n", tcpObj->psh ? "Set" : "Not Set");
}

void tcp_PrintRST(tcpclass_t *tcpObj) {
    printf("RST Flag: %s\n", tcpObj->rst ? "Set" : "Not Set");
}

void tcp_PrintSYN(tcpclass_t *tcpObj) {
    printf("SYN Flag: %s\n", tcpObj->syn ? "Set" : "Not Set");
}

void tcp_PrintFIN(tcpclass_t *tcpObj) {
    printf("FIN Flag: %s\n", tcpObj->fin ? "Set" : "Not Set");
}

void tcp_PrintWindowSize(tcpclass_t *tcpObj) {
    printf("Window Size: %u\n", ntohs(tcpObj->window));
}

void tcp_PrintChecksum(tcpclass_t *tcpObj) {
    printf("Checksum: 0x%04X\n", ntohs(tcpObj->check));
}

void tcp_PrintUrgPtr(tcpclass_t *tcpObj) {
    printf("Urgent Pointer: %u\n", ntohs(tcpObj->urg_ptr));
}

// Print All TCP Data
void tcp_PrintAllTCPData(tcpclass_t *tcpObj) {
    if (tcpObj == NULL) {
        printf("TCP object is NULL.\n");
        return;
    }
    tcpObj->PrintSourcePort(tcpObj);
    tcpObj->PrintDestPort(tcpObj);
    tcpObj->PrintSeqNum(tcpObj);
    tcpObj->PrintAckNum(tcpObj);
    tcpObj->PrintOffset(tcpObj);
    tcpObj->PrintReserved(tcpObj);
    tcpObj->PrintURG(tcpObj);
    tcpObj->PrintACK(tcpObj);
    tcpObj->PrintPSH(tcpObj);
    tcpObj->PrintRST(tcpObj);
    tcpObj->PrintSYN(tcpObj);
    tcpObj->PrintFIN(tcpObj);
    tcpObj->PrintWindowSize(tcpObj);
    tcpObj->PrintChecksum(tcpObj);
    tcpObj->PrintUrgPtr(tcpObj);
}


tcpclass_t* ConstructTCPObject(const  unsigned char *packet) {
    tcpclass_t* tcpObj = (tcpclass_t *)malloc(sizeof(tcpclass_t));
    if (!tcpObj) {
        fprintf(stderr, "Failed to allocate memory for TCP object.\n");
        return NULL;
    }

    ipclass_t *ipObj = ConstructIPObject(packet);
    if (!ipObj) {
        free(tcpObj);
        fprintf(stderr, "Failed to construct IP object.\n");
        return NULL;
    }
    struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + (ipObj->ihl)*4);  

    tcpObj->ipPart = ipObj;

    tcpObj->source = tcp_header->source;
    tcpObj->dest = tcp_header->dest;
    tcpObj->seq = tcp_header->seq;
    tcpObj->ack_seq = tcp_header->ack_seq;
    
    tcpObj->offset = tcp_header->doff;

    tcpObj->reserved = tcp_header->res1;
    tcpObj->urg = tcp_header->urg;
    tcpObj->ack = tcp_header->ack;
    tcpObj->psh = tcp_header->psh;
    tcpObj->rst = tcp_header->rst;
    tcpObj->syn = tcp_header->syn;
    tcpObj->fin = tcp_header->fin;

    tcpObj->window = tcp_header->window;
    tcpObj->check = tcp_header->check;
    tcpObj->urg_ptr = tcp_header->urg_ptr;

    // Assign function pointers
    tcpObj->PrintSourcePort = tcp_PrintSourcePort;
    tcpObj->PrintDestPort = tcp_PrintDestPort;
    tcpObj->PrintSeqNum = tcp_PrintSeqNum;
    tcpObj->PrintAckNum = tcp_PrintAckNum;
    tcpObj->PrintOffset = tcp_PrintOffset;
    tcpObj->PrintReserved = tcp_PrintReserved;

    // Assign flag functions
    tcpObj->PrintURG = tcp_PrintURG;
    tcpObj->PrintACK = tcp_PrintACK;
    tcpObj->PrintPSH = tcp_PrintPSH;
    tcpObj->PrintRST = tcp_PrintRST;
    tcpObj->PrintSYN = tcp_PrintSYN;
    tcpObj->PrintFIN = tcp_PrintFIN;

    tcpObj->PrintWindowSize = tcp_PrintWindowSize;
    tcpObj->PrintChecksum = tcp_PrintChecksum;
    tcpObj->PrintUrgPtr = tcp_PrintUrgPtr;

    tcpObj->PrintAllTCPDetails = tcp_PrintAllTCPData;


    return tcpObj;
}


tcpclass_t* DeconstructTCPObject(tcpclass_t *tcpclassObj){
    if(tcpclassObj){
        DeconstructIPObject(tcpclassObj->ipPart);
        free(tcpclassObj);
    }
}