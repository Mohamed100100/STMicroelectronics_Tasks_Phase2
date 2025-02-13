#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <netinet/udp.h>


#include "UDPclass.h"

// Print functions
static void udp_PrintSourcePort(udpclass_t *udpObj) {
    printf("Source Port: %u\n", ntohs(udpObj->source));
}

static void udp_PrintDestPort(udpclass_t *udpObj) {
    printf("Destination Port: %u\n", ntohs(udpObj->dest));
}

static void udp_PrintLength(udpclass_t *udpObj) {
    printf("UDP Length: %u\n", ntohs(udpObj->len));
}

static void udp_PrintChecksum(udpclass_t *udpObj) {
    printf("UDP Checksum: 0x%x\n", ntohs(udpObj->check));
}

static void udp_PrintAllUDPDetails(udpclass_t *udpObj){
    if (udpObj == NULL) {
        printf("UDP object is NULL.\n");
        return;
    }
    udpObj->PrintSourcePort(udpObj);
    udpObj->PrintDestPort(udpObj);
    udpObj->PrintLength(udpObj);
    udpObj->PrintChecksum(udpObj);
}


// Constructor function
udpclass_t* ConstructUDPObject(const unsigned char  *packet) {
    udpclass_t *udpObj = (udpclass_t *)malloc(sizeof(udpclass_t));
    if (!udpObj) {
        fprintf(stderr, "Failed to allocate memory for UDP object.\n");
        return NULL;
    }
    ipclass_t *ipobj = ConstructIPObject(packet);
    
    if (!ipobj) {
        free(udpObj);
        fprintf(stderr, "Failed to construct IP object.\n");
        return NULL;
    }

    struct udphdr *udp_header = (struct udphdr *)(packet + 14 + (ipobj->ihl)*4);;

    udpObj->source = udp_header->source;
    udpObj->dest = udp_header->dest;
    udpObj->len = udp_header->len;
    udpObj->check = udp_header->check;

    udpObj->ipPart = ipobj;

    // Assign print functions
    udpObj->PrintSourcePort = udp_PrintSourcePort;
    udpObj->PrintDestPort = udp_PrintDestPort;
    udpObj->PrintLength = udp_PrintLength;
    udpObj->PrintChecksum = udp_PrintChecksum;
    udpObj->PrintAllUDPDetails = udp_PrintAllUDPDetails;

    return udpObj;
}

void DeconstructUDPObject(udpclass_t *udpclassObj){
    if(udpclassObj){
        DeconstructIPObject(udpclassObj->ipPart);
        free(udpclassObj);
    }
}



