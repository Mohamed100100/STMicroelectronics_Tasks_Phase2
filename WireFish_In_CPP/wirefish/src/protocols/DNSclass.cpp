#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#include "DNSclass.h"

void dnsclass_t::PrintTransactionID() {
    printf("Transaction ID: 0x%04X\n", ntohs(this->transaction_id));
}

void dnsclass_t::PrintQuestions( ) {
    printf("Number of Questions: %u\n", ntohs(this->questions));
}

void dnsclass_t::PrintAnswers( ) {
    printf("Number of Answers: %u\n", ntohs(this->answers));
}

void dnsclass_t::PrintAuthorityRR( ) {
    printf("Number of Authority Resource Records: %u\n", ntohs(this->authority_rr));
}

void dnsclass_t::PrintAdditionalRR( ) {
    printf("Number of Additional Resource Records: %u\n", ntohs(this->additional_rr));
}


void dnsclass_t::PrintFlags( ) {
    uint16_t flags = ntohs(this->flags);

    printf("Flags: 0x%04X\n", flags);

    // QR bit (0: query, 1: response)
    printf("QR: %s\n", (flags & 0x8000) ? "Response" : "Query");

    // OPCODE (4 bits)
    uint8_t opcode = (flags >> 11) & 0xF;
    printf("Opcode: %d ", opcode);
    switch (opcode) {
        case 0: printf("(Standard Query)\n"); break;
        case 1: printf("(Inverse Query)\n"); break;
        case 2: printf("(Server Status Request)\n"); break;
        default: printf("(Unknown Opcode)\n"); break;
    }

    // AA bit
    printf("Authoritative Answer (AA): %s\n", (flags & 0x0400) ? "Yes" : "No");

    // TC bit
    printf("Truncated (TC): %s\n", (flags & 0x0200) ? "Yes" : "No");

    // RD bit
    printf("Recursion Desired (RD): %s\n", (flags & 0x0100) ? "Yes" : "No");

    // RA bit
    printf("Recursion Available (RA): %s\n", (flags & 0x0080) ? "Yes" : "No");

    // Z field (1 bit reserved)
    printf("Z (Reserved): %d\n", (flags & 0x0040) >> 6);

    // AD bit
    printf("Authentic Data (AD): %s\n", (flags & 0x0020) ? "Yes" : "No");

    // CD bit
    printf("Checking Disabled (CD): %s\n", (flags & 0x0010) ? "Yes" : "No");

    // RCODE (4 bits)
    uint8_t rcode = flags & 0xF;
    printf("Response Code (RCODE): %d ", rcode);
    switch (rcode) {
        case 0: printf("(No Error)\n"); break;
        case 1: printf("(Format Error)\n"); break;
        case 2: printf("(Server Failure)\n"); break;
        case 3: printf("(Nonexistent Domain)\n"); break;
        default: printf("(Unknown Error)\n"); break;
    }
}
void dnsclass_t::PrintAllDNSDetails() {

    this->PrintTransactionID();
    this->PrintFlags();
    this->PrintQuestions();
    this->PrintAnswers();
    this->PrintAuthorityRR();
    this->PrintAdditionalRR();
}

dnsclass_t::dnsclass_t(){}
dnsclass_t::dnsclass_t(const unsigned char *packet) :udpclass_t(packet){

    struct iphdr *ip_header = (struct iphdr *)(packet + 14); 

    this->data_start = packet + 14 + (ip_header->ihl)*4 + sizeof(struct udphdr) ;


    // Parse header fields
    this->transaction_id = *(uint16_t *)(this->data_start);
    this->flags = *(uint16_t *)(this->data_start + 2);
    this->questions = *(uint16_t *)(this->data_start + 4);
    this->answers = *(uint16_t *)(this->data_start + 6);
    this->authority_rr = *(uint16_t *)(this->data_start + 8);
    this->additional_rr = *(uint16_t *)(this->data_start + 10);
}


