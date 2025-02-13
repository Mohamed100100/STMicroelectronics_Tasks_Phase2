#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pcap.h>

#include "ICMPclass.h"

static void icmp_PrintType(icmpclass_t *ICMPObj) {
    printf("Type: %u ", ICMPObj->type);
    switch (ICMPObj->type) {
        case ICMP_ECHO: printf("(Echo Request)\n"); break;
        case ICMP_ECHOREPLY: printf("(Echo Reply)\n"); break;
        case ICMP_DEST_UNREACH: printf("(Destination Unreachable)\n"); break;
        case ICMP_TIME_EXCEEDED: printf("(Time Exceeded)\n"); break;
        default: printf("(Unknown Type)\n");
    }
}

static void icmp_PrintCode(icmpclass_t *ICMPObj) {
    printf("Code: %u\n", ICMPObj->code);
}

static void icmp_PrintChecksum(icmpclass_t *ICMPObj) {
    printf("Checksum: 0x%X\n", ntohs(ICMPObj->checksum));
}

static void icmp_PrintIdentifier(icmpclass_t *ICMPObj) {
    printf("Identifier: %u\n", ntohs(ICMPObj->identifier));
}

static void icmp_PrintSequenceNumber(icmpclass_t *ICMPObj) {
    printf("Sequence Number: %u\n", ntohs(ICMPObj->sequence));
}

static void icmp_PrintAllICMPDetails(icmpclass_t *ICMPObj) {
    if (ICMPObj == NULL) {
        printf("ICMP object is NULL.\n");
        return;
    }

    ICMPObj->PrintType(ICMPObj);
    ICMPObj->PrintCode(ICMPObj);
    ICMPObj->PrintChecksum(ICMPObj);
    ICMPObj->PrintIdentifier(ICMPObj);
    ICMPObj->PrintSequenceNumber(ICMPObj);
}

icmpclass_t* ConstructICMPObject(const unsigned char *packet) {
    icmpclass_t* icmpclassObj = (icmpclass_t *)malloc(sizeof(icmpclass_t));
    if (!icmpclassObj) {
        fprintf(stderr, "Failed to allocate memory for ICMP object.\n");
        return NULL;
    }
    ipclass_t *ipObj = ConstructIPObject(packet);    
    
    if (!ipObj) {
        free(icmpclassObj);
        fprintf(stderr, "Failed to construct IP object.\n");
        return NULL;
    }
    struct icmphdr *icmp_header = (struct icmphdr *)(packet + 14 + (ipObj->ihl * 4));  

    icmpclassObj->ipPart = ipObj;                  

    icmpclassObj->type = icmp_header->type;
    icmpclassObj->code = icmp_header->code;
    icmpclassObj->checksum = icmp_header->checksum;
    icmpclassObj->identifier = icmp_header->un.echo.id;
    icmpclassObj->sequence = icmp_header->un.echo.sequence;

    // Assign function pointers
    icmpclassObj->PrintType = icmp_PrintType;
    icmpclassObj->PrintCode = icmp_PrintCode;
    icmpclassObj->PrintChecksum = icmp_PrintChecksum;
    icmpclassObj->PrintIdentifier = icmp_PrintIdentifier;
    icmpclassObj->PrintSequenceNumber = icmp_PrintSequenceNumber;

    icmpclassObj->PrintAllICMPDetails = icmp_PrintAllICMPDetails;

    return icmpclassObj;
}

void DeconstructICMPObject(icmpclass_t *icmpclassObj) {
    DeconstructIPObject(icmpclassObj->ipPart);

    free(icmpclassObj);
}

