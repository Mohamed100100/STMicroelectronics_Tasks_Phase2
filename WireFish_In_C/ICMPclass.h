#ifndef ICMPCLASS_H
#define ICMPCLASS_H

#include <stdint.h>
#include "IPclass.h"

typedef struct icmpclass {
    ipclass_t *ipPart;                               // inheritance

    uint8_t type;           // ICMP message type
    uint8_t code;           // ICMP message code
    uint16_t checksum;      // ICMP checksum
    uint16_t identifier;    // Identifier for Echo Request/Reply
    uint16_t sequence;      // Sequence number for Echo Request/Reply

    // Function pointers
    void (*PrintType)(struct icmpclass *ICMPObj);
    void (*PrintCode)(struct icmpclass *ICMPObj);
    void (*PrintChecksum)(struct icmpclass *ICMPObj);
    void (*PrintIdentifier)(struct icmpclass *ICMPObj);
    void (*PrintSequenceNumber)(struct icmpclass *ICMPObj);

    // method to call all print functions
    void(*PrintAllICMPDetails)(struct icmpclass *ICMPObj);
} icmpclass_t;

icmpclass_t* ConstructICMPObject(const unsigned char *packet);
void DeconstructICMPObject(icmpclass_t *icmpclassObj);

#endif
