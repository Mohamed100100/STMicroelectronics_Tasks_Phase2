#ifndef _DNSCLASS_H_
#define _DNSCLASS_H_

#include <stdint.h>
#include "UDPclass.h"


typedef struct dnsclass_t {
    udpclass_t *udpPart;              // Associated UDP object

    const unsigned char *data_start;  // Pointer to the start of DNS payload

    // DNS header information
    uint16_t transaction_id;          // Transaction ID
    uint16_t flags;                   // Flags
    uint16_t questions;               // Number of Questions
    uint16_t answers;                 // Number of Answers
    uint16_t authority_rr;            // Number of Authority Resource Records
    uint16_t additional_rr;           // Number of Additional Resource Records

    // Method function pointers
    void (*PrintTransactionID)(struct dnsclass_t *DNSObj);
    void (*PrintFlags)(struct dnsclass_t *DNSObj);
    void (*PrintQuestions)(struct dnsclass_t *DNSObj);
    void (*PrintAnswers)(struct dnsclass_t *DNSObj);
    void (*PrintAuthorityRR)(struct dnsclass_t *DNSObj);
    void (*PrintAdditionalRR)(struct dnsclass_t *DNSObj);
  
    // method to call all print functions
    void(*PrintAllDNSDetails)(struct dnsclass_t *DNSObj);
} dnsclass_t;



dnsclass_t* ConstructDNSObject(const unsigned char *packet) ;
void DeconstructDNSObject(dnsclass_t *dnsObj) ;

#endif    //_DNSCLASS_H_