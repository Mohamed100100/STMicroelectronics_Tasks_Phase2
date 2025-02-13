#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/udp.h>

#include "DNSclass.h"



static void dns_PrintTransactionID(dnsclass_t *DNSObj) {
    printf("Transaction ID: 0x%04X\n", ntohs(DNSObj->transaction_id));
}

static void dns_PrintQuestions(dnsclass_t *DNSObj) {
    printf("Number of Questions: %u\n", ntohs(DNSObj->questions));
}

static void dns_PrintAnswers(dnsclass_t *DNSObj) {
    printf("Number of Answers: %u\n", ntohs(DNSObj->answers));
}

static void dns_PrintAuthorityRR(dnsclass_t *DNSObj) {
    printf("Number of Authority Resource Records: %u\n", ntohs(DNSObj->authority_rr));
}

static void dns_PrintAdditionalRR(dnsclass_t *DNSObj) {
    printf("Number of Additional Resource Records: %u\n", ntohs(DNSObj->additional_rr));
}


static void dns_PrintFlags(dnsclass_t *DNSObj) {
    uint16_t flags = ntohs(DNSObj->flags);

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
static void dns_PrintAllDNSDetails(dnsclass_t *DNSObj) {
    if (DNSObj == NULL) {
        printf("DNS object is NULL.\n");
        return;
    }
    DNSObj->PrintTransactionID(DNSObj);
    DNSObj->PrintFlags(DNSObj);
    DNSObj->PrintQuestions(DNSObj);
    DNSObj->PrintAnswers(DNSObj);
    DNSObj->PrintAuthorityRR(DNSObj);
    DNSObj->PrintAdditionalRR(DNSObj);
}


dnsclass_t* ConstructDNSObject(const unsigned char *packet) {
    dnsclass_t *dnsObj = (dnsclass_t *)malloc(sizeof(dnsclass_t));
    if (!dnsObj) {
        fprintf(stderr, "Failed to allocate memory for DNS object.\n");
        return NULL;
    }
    udpclass_t *udpObj = ConstructUDPObject(packet);
    if (!udpObj) {
        free(dnsObj);
        fprintf(stderr, "Failed to construct UDP object.\n");
        return NULL;
    }
    dnsObj->data_start = packet + 14 + (udpObj->ipPart->ihl)*4 + sizeof(struct udphdr) ;

    dnsObj->udpPart = udpObj;

    // Parse header fields
    dnsObj->transaction_id = *(uint16_t *)(dnsObj->data_start);
    dnsObj->flags = *(uint16_t *)(dnsObj->data_start + 2);
    dnsObj->questions = *(uint16_t *)(dnsObj->data_start + 4);
    dnsObj->answers = *(uint16_t *)(dnsObj->data_start + 6);
    dnsObj->authority_rr = *(uint16_t *)(dnsObj->data_start + 8);
    dnsObj->additional_rr = *(uint16_t *)(dnsObj->data_start + 10);

    dnsObj->PrintTransactionID = dns_PrintTransactionID;
    dnsObj->PrintFlags = dns_PrintFlags;
    dnsObj->PrintQuestions = dns_PrintQuestions;
    dnsObj->PrintAnswers = dns_PrintAnswers;
    dnsObj->PrintAuthorityRR = dns_PrintAuthorityRR;
    dnsObj->PrintAdditionalRR = dns_PrintAdditionalRR;

    dnsObj->PrintAllDNSDetails = dns_PrintAllDNSDetails;

    return dnsObj;
}

void DeconstructDNSObject(dnsclass_t *dnsObj) {
    if (dnsObj) {
        DeconstructUDPObject(dnsObj->udpPart);
        free(dnsObj);
    }
}
