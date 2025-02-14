#ifndef ICMPCLASS_H
#define ICMPCLASS_H

#include <stdint.h>
#include "IPclass.h"

class icmpclass_t : public ipclass_t
{
    public :
    uint8_t type;           // ICMP message type
    uint8_t code;           // ICMP message code
    uint16_t checksum;      // ICMP checksum
    uint16_t identifier;    // Identifier for Echo Request/Reply
    uint16_t sequence;      // Sequence number for Echo Request/Reply

    icmpclass_t();
    icmpclass_t(const unsigned char *packet);

    // Function pointers
    void PrintType();
    void PrintCode();
    void PrintChecksum();
    void PrintIdentifier();
    void PrintSequenceNumber();

    // method to call all print functions
    void PrintAllICMPDetails();
};


#endif
