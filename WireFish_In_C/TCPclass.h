#ifndef _TCPCLASS_H_
#define _TCPCLASS_H_

#include <stdint.h>
#include "IPclass.h"

typedef struct tcpclass {
    ipclass_t *ipPart;  // Inheritance from IP class

    uint16_t source;     // Source port
    uint16_t dest;       // Destination port
    uint32_t seq;        // Sequence number
    uint32_t ack_seq;    // Acknowledgment number
    uint8_t offset;      // Data offset (header length)
    uint8_t reserved;    // Reserved (3 bits, but stored in a whole byte)
    
    // Individual TCP flag fields
    uint8_t urg;         // URG flag (1 bit)
    uint8_t ack;         // ACK flag (1 bit)
    uint8_t psh;         // PSH flag (1 bit)
    uint8_t rst;         // RST flag (1 bit)
    uint8_t syn;         // SYN flag (1 bit)
    uint8_t fin;         // FIN flag (1 bit)

    uint16_t window;     // Window size
    uint16_t check;      // Checksum
    uint16_t urg_ptr;    // Urgent pointer

    // Method pointers for printing details
    void (*PrintSourcePort)(struct tcpclass *);
    void (*PrintDestPort)(struct tcpclass *);
    void (*PrintSeqNum)(struct tcpclass *);
    void (*PrintAckNum)(struct tcpclass *);
    void (*PrintOffset)(struct tcpclass *);
    void (*PrintReserved)(struct tcpclass *);
    
    // Individual flag print methods
    void (*PrintURG)(struct tcpclass *);
    void (*PrintACK)(struct tcpclass *);
    void (*PrintPSH)(struct tcpclass *);
    void (*PrintRST)(struct tcpclass *);
    void (*PrintSYN)(struct tcpclass *);
    void (*PrintFIN)(struct tcpclass *);

    void (*PrintWindowSize)(struct tcpclass *);
    void (*PrintChecksum)(struct tcpclass *);
    void (*PrintUrgPtr)(struct tcpclass *);

    void (*PrintAllTCPDetails)(struct tcpclass *);

} tcpclass_t;



tcpclass_t* ConstructTCPObject(const  unsigned char *packet);

tcpclass_t* DeconstructTCPObject(tcpclass_t *tcpclassObj) ;

#endif //_TCPCLASS_H_