#ifndef _DNSCLASS_H_
#define _DNSCLASS_H_

#include <stdint.h>
#include "UDPclass.h"


class dnsclass_t : public udpclass_t
 {
    public :
    const unsigned char *data_start;  // Pointer to the start of DNS payload

    // DNS header information
    uint16_t transaction_id;          // Transaction ID
    uint16_t flags;                   // Flags
    uint16_t questions;               // Number of Questions
    uint16_t answers;                 // Number of Answers
    uint16_t authority_rr;            // Number of Authority Resource Records
    uint16_t additional_rr;           // Number of Additional Resource Records

    dnsclass_t();
    dnsclass_t(const unsigned char *packet);

    // Method function pointers
    void PrintTransactionID();
    void PrintFlags();
    void PrintQuestions();
    void PrintAnswers();
    void PrintAuthorityRR();
    void PrintAdditionalRR();
  
    // method to call all print functions
    void PrintAllDNSDetails();
} ;


#endif    //_DNSCLASS_H_