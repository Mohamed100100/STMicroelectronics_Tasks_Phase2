#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pcap.h>

#include "ICMPclass.h"

void icmpclass_t::PrintType() {

    printf("Type: %u ", this->type);
    switch (this->type) {
        case ICMP_ECHO: printf("(Echo Request)\n"); break;
        case ICMP_ECHOREPLY: printf("(Echo Reply)\n"); break;
        case ICMP_DEST_UNREACH: printf("(Destination Unreachable)\n"); break;
        case ICMP_TIME_EXCEEDED: printf("(Time Exceeded)\n"); break;
        default: printf("(Unknown Type)\n");
    }
}

void icmpclass_t::PrintCode() {
    printf("Code: %u\n", this->code);
}

void icmpclass_t::PrintChecksum( ) {
    printf("Checksum: 0x%X\n", ntohs(this->checksum));
}

void icmpclass_t::PrintIdentifier( ) {
    printf("Identifier: %u\n", ntohs(this->identifier));
}

void icmpclass_t::PrintSequenceNumber( ) {
    printf("Sequence Number: %u\n", ntohs(this->sequence));
}
void icmpclass_t::PrintAllICMPDetails( ) {
    this->PrintType();
    this->PrintCode();
    this->PrintChecksum();
    this->PrintIdentifier();
    this->PrintSequenceNumber();
}
icmpclass_t::icmpclass_t(){}
 icmpclass_t::icmpclass_t(const unsigned char *packet) :ipclass_t(packet){

    struct iphdr *ip_header = (struct iphdr *)(packet + 14); 
    struct icmphdr *icmp_header = (struct icmphdr *)(packet + 14 + (ip_header->ihl * 4));                 

    this->type = icmp_header->type;
    this->code = icmp_header->code;
    this->checksum = icmp_header->checksum;
    this->identifier = icmp_header->un.echo.id;
    this->sequence = icmp_header->un.echo.sequence;
}




