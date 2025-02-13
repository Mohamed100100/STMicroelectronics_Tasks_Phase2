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

// Static functions
static void ip_PrintTheTotalLength(ipclass_t *IPObj) {
    printf("Total Length: %u bytes\n", ntohs(IPObj->tot_len));
}

static void ip_PrintTheHeaderLength(ipclass_t *IPObj) {
    printf("Header Length: %u DWORDS (%u bytes)\n", IPObj->ihl, IPObj->ihl * 4);
}

static void ip_PrintTheID(ipclass_t *IPObj) {
    printf("Identification: %u\n", ntohs(IPObj->id));
}

static void ip_PrintTheVersion(ipclass_t *IPObj) {
    printf("IP Version: %d\n", IPObj->version);
}


static void ip_PrintTheFragmentInfo(ipclass_t *IPObj) {
    uint16_t frag_off_flags = ntohs(IPObj->frag_off);

    // Extract flags and fragment offset
    uint16_t fragment_offset = frag_off_flags & 0x1FFF;     // Lower 13 bits
    uint8_t df_flag = (frag_off_flags & 0x4000) >> 14;      // Bit 14
    uint8_t mf_flag = (frag_off_flags & 0x2000) >> 13;      // Bit 13

    printf("Fragment Offset: %u\n", fragment_offset);
    printf("Flags:\n");
    printf("  Don't Fragment (DF): %u\n", df_flag);
    printf("  More Fragments (MF): %u\n", mf_flag);
}


static void ip_PrintTheTTL(ipclass_t *IPObj) {
    printf("Time to Live (TTL): %u\n", IPObj->ttl);
}

static void ip_PrintTheProtocol(ipclass_t *IPObj) {
    printf("Protocol: %u - ", IPObj->protocol);

    switch (IPObj->protocol) {
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


static void ip_PrintTheCheckSum(ipclass_t *IPObj) {
    printf("Header Checksum: 0x%04x\n", ntohs(IPObj->checksum));
}

static void ip_PrintTheSrcIP(ipclass_t *IPObj) {
    struct in_addr src_addr;
    src_addr.s_addr = IPObj->src_ip;
    printf("Source IP: %s\n", inet_ntoa(src_addr));
}

static void ip_PrintTheDstIP(ipclass_t *IPObj) {
    struct in_addr dest_addr;
    dest_addr.s_addr = IPObj->dest_ip;
    printf("Destination IP: %s\n", inet_ntoa(dest_addr));
}

static void ip_PrintAllIPDetails(ipclass_t *IPObj){    
    if (IPObj == NULL) {
        printf("IP object is NULL.\n");
        return;
    }
  
    IPObj->PrintTheVersion(IPObj);
    IPObj->PrintTheTotalLength(IPObj);
    IPObj->PrintTheHeaderLength(IPObj);
    IPObj->PrintTheID(IPObj);
    IPObj->PrintTheFragmentInfo(IPObj);
    IPObj->PrintTheTTL(IPObj);
    IPObj->PrintTheProtocol(IPObj);
    IPObj->PrintTheCheckSum(IPObj);
    IPObj->PrintTheSrcIP(IPObj);
    IPObj->PrintTheDstIP(IPObj);
}

ipclass_t* ConstructIPObject(const  unsigned char *packet) {
    ipclass_t *ipclassObj = (ipclass_t *)malloc(sizeof(ipclass_t));
    if (!ipclassObj) {
        fprintf(stderr, "Failed to allocate memory for IP object.\n");
        return NULL;
    }
    struct iphdr *ip_header = (struct iphdr *)(packet + 14); // Ethernet header is 14 bytes

    ipclassObj->version = ip_header->version;
    ipclassObj->ihl = ip_header->ihl;
    ipclassObj->tos = ip_header->tos;
    ipclassObj->tot_len = ip_header->tot_len;
    ipclassObj->id = ip_header->id;
    ipclassObj->frag_off = ip_header->frag_off;
    ipclassObj->ttl = ip_header->ttl;
    ipclassObj->protocol = ip_header->protocol;
    ipclassObj->checksum = ip_header->check;
    ipclassObj->src_ip = ip_header->saddr;
    ipclassObj->dest_ip = ip_header->daddr;

    ipclassObj->PrintTheVersion = ip_PrintTheVersion;
    ipclassObj->PrintTheCheckSum = ip_PrintTheCheckSum;
    ipclassObj->PrintTheDstIP = ip_PrintTheDstIP;
    ipclassObj->PrintTheFragmentInfo = ip_PrintTheFragmentInfo;
    ipclassObj->PrintTheProtocol = ip_PrintTheProtocol;
    ipclassObj->PrintTheSrcIP = ip_PrintTheSrcIP;
    ipclassObj->PrintTheID = ip_PrintTheID;
    ipclassObj->PrintTheTTL= ip_PrintTheTTL;
    ipclassObj->PrintTheHeaderLength= ip_PrintTheHeaderLength;
    ipclassObj->PrintTheTotalLength = ip_PrintTheTotalLength;
    ipclassObj->PrintAllIPDetails = ip_PrintAllIPDetails;

    return ipclassObj;
}

void DeconstructIPObject(ipclass_t *ipclassObj){
    if(ipclassObj){
        free(ipclassObj);
    }
}