#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> // For byte order functions (ntohs)
#include <arpa/inet.h>  // For IP address functions
#include <netinet/ip.h>

#include <pcap.h>
#include <pcap/pcap.h>

#include "IPclass.h"

void ipclass_t::PrintTheTotalLength() {
    printf("Total Length: %u bytes\n", ntohs(this->tot_len));
}

void ipclass_t::PrintTheHeaderLength() {
    printf("Header Length: %u DWORDS (%u bytes)\n", this->ihl, this->ihl * 4);
}

void ipclass_t::PrintTheID() {
    printf("Identification: %u\n", ntohs(this->id));
}

void ipclass_t::PrintTheVersion() {
    printf("IP Version: %d\n", this->version);
}


void ipclass_t::PrintTheFragmentInfo() {
    uint16_t frag_off_flags = ntohs(this->frag_off);

    // Extract flags and fragment offset
    uint16_t fragment_offset = frag_off_flags & 0x1FFF;     // Lower 13 bits
    uint8_t df_flag = (frag_off_flags & 0x4000) >> 14;      // Bit 14
    uint8_t mf_flag = (frag_off_flags & 0x2000) >> 13;      // Bit 13

    printf("Fragment Offset: %u\n", fragment_offset);
    printf("Flags:\n");
    printf("  Don't Fragment (DF): %u\n", df_flag);
    printf("  More Fragments (MF): %u\n", mf_flag);
}


void ipclass_t::PrintTheTTL() {
    printf("Time to Live (TTL): %u\n", this->ttl);
}

void ipclass_t::PrintTheProtocol() {
    printf("Protocol: %u - ", this->protocol);

    switch (this->protocol) {
        case 1:
            printf("ICMP (Internet Control Message Protocol)\n");
            break;
        case 6:
            printf("TCP (Transmission Control Protocol)\n");
            break;
        case 17:
            printf("UDP (User Datagram Protocol)\n");
            break;
        case 2:
            printf("IGMP (Internet Group Management Protocol)\n");
            break;
        case 89:
            printf("OSPF (Open Shortest Path First)\n");
            break;
        case 41:
            printf("IPv6 (IPv6 encapsulation)\n");
            break;
        case 50:
            printf("ESP (Encapsulating Security Payload)\n");
            break;
        case 51:
            printf("AH (Authentication Header)\n");
            break;
        default:
            printf("Unknown or Unsupported Protocol\n");
            break;
    }
}


void ipclass_t::PrintTheCheckSum() {
    printf("Header Checksum: 0x%04x\n", ntohs(this->checksum));
}

void ipclass_t::PrintTheSrcIP() {
    struct in_addr src_addr;
    src_addr.s_addr = this->src_ip;
    printf("Source IP: %s\n", inet_ntoa(src_addr));
}

void ipclass_t::PrintTheDstIP() {
    struct in_addr dest_addr;
    dest_addr.s_addr = this->dest_ip;
    printf("Destination IP: %s\n", inet_ntoa(dest_addr));
}

void ipclass_t::PrintAllIPDetails(){    
    this->PrintTheVersion();
    this->PrintTheTotalLength();
    this->PrintTheHeaderLength();
    this->PrintTheID();
    this->PrintTheFragmentInfo();
    this->PrintTheTTL();
    this->PrintTheProtocol();
    this->PrintTheCheckSum();
    this->PrintTheSrcIP();
    this->PrintTheDstIP();
}

ipclass_t::ipclass_t(const  unsigned char *packet) {
    struct iphdr *ip_header = (struct iphdr *)(packet + 14); // Ethernet header is 14 bytes

    this->version = ip_header->version;
    this->ihl = ip_header->ihl;
    this->tos = ip_header->tos;
    this->tot_len = ip_header->tot_len;
    this->id = ip_header->id;
    this->frag_off = ip_header->frag_off;
    this->ttl = ip_header->ttl;
    this->protocol = ip_header->protocol;
    this->checksum = ip_header->check;
    this->src_ip = ip_header->saddr;
    this->dest_ip = ip_header->daddr;
}
// defualt constructor 
ipclass_t::ipclass_t(){}


