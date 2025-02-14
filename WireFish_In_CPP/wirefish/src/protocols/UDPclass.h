#ifndef _UDPCLASS_H_
#define _UDPCLASS_H_

#include <stdint.h>
#include "IPclass.h"

class udpclass_t : public ipclass_t
{
    public:
    uint16_t source;    // Source port
    uint16_t dest;      // Destination port
    uint16_t len;       // Datagram length (header + data)
    uint16_t check;     // Checksum (error checking)

    udpclass_t();
    udpclass_t(const unsigned char  *packet);

    // Print methods
    void PrintSourcePort();
    void PrintDestPort();
    void PrintLength();
    void PrintChecksum();

    // method to call all print functions
    void PrintAllUDPDetails();
};

#endif //_UDPCLASS_H_
