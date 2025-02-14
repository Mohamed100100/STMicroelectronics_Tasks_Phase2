#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>


#include "UDPclass.h"

// Print functions
void udpclass_t::PrintSourcePort() {
    printf("Source Port: %u\n", ntohs(this->source));
}

void udpclass_t::PrintDestPort() {
    printf("Destination Port: %u\n", ntohs(this->dest));
}

void udpclass_t::PrintLength( ) {
    printf("UDP Length: %u\n", ntohs(this->len));
}

void udpclass_t::PrintChecksum() {
    printf("UDP Checksum: 0x%x\n", ntohs(this->check));
}

void udpclass_t::PrintAllUDPDetails(){
    this->PrintSourcePort();
    this->PrintDestPort();
    this->PrintLength();
    this->PrintChecksum();
}


// Constructor function
 udpclass_t::udpclass_t(){}
 udpclass_t::udpclass_t(const unsigned char  *packet):ipclass_t(packet) {    

    struct iphdr *ip_header = (struct iphdr *)(packet + 14); 
    struct udphdr *udp_header = (struct udphdr *)(packet + 14 + (ip_header->ihl)*4);

    this->source = udp_header->source;
    this->dest = udp_header->dest;
    this->len = udp_header->len;
    this->check = udp_header->check;
}



