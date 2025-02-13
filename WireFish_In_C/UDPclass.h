#ifndef _UDPCLASS_H_
#define _UDPCLASS_H_

#include <stdint.h>
#include "IPclass.h"
typedef struct udpclass udpclass_t;
struct udpclass {
    ipclass_t *ipPart;                               // inheritance

    uint16_t source;    // Source port
    uint16_t dest;      // Destination port
    uint16_t len;       // Datagram length (header + data)
    uint16_t check;     // Checksum (error checking)
    
    // Print methods
    void (*PrintSourcePort)(udpclass_t *udpObj);
    void (*PrintDestPort)(udpclass_t *udpObj);
    void (*PrintLength)(udpclass_t *udpObj);
    void (*PrintChecksum)(udpclass_t *udpObj);

    // method to call all print functions
    void (*PrintAllUDPDetails)(udpclass_t *udpObj);
};


udpclass_t* ConstructUDPObject(const unsigned char  *packet) ;

void DeconstructUDPObject(udpclass_t *udpclassObj);


#endif //_UDPCLASS_H_
