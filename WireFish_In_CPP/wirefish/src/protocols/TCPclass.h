#ifndef _TCPCLASS_H_
#define _TCPCLASS_H_

#include <stdint.h>
#include "IPclass.h"

class tcpclass_t :public ipclass_t         //inhirit
{
    public:
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

    tcpclass_t();
    tcpclass_t(const  unsigned char *packet);

    void PrintSourcePort();
    void PrintDestPort();
    void PrintSeqNum();
    void PrintAckNum();
    void PrintOffset();
    void PrintReserved();
    
    // Individual flag print methods
    void PrintURG();
    void PrintACK();
    void PrintPSH();
    void PrintRST();
    void PrintSYN();
    void PrintFIN();

    void PrintWindowSize();
    void PrintChecksum();
    void PrintUrgPtr();

    void PrintAllTCPDetails();

};


#endif //_TCPCLASS_H_