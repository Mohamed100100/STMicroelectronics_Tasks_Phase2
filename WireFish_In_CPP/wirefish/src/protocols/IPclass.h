#ifndef _IPCLASS_H
#define _IPCLASS_H

#include <stdint.h>


class ipclass_t{
    public:
    uint8_t version;
    uint8_t ihl;                            // Internet Header Length (number of 32-bit words)
    uint8_t tos;                            // Type of Service
    uint16_t tot_len;                       // Total Length (header + data in bytes)
    uint16_t id;                            // Identification
    uint16_t frag_off;                      // Fragment Offset and flags
    uint8_t ttl;                            // Time to Live
    uint8_t protocol;                       //Transport Layer Protocol (TCP, UDP, ICMP, etc.)
    uint16_t checksum;                      // Header checksum
    uint32_t src_ip;                        // Source IP address
    uint32_t dest_ip;                       // Destination IP address
    
    ipclass_t();
    ipclass_t(const  unsigned char *packet);

    void PrintTheVersion();
    void PrintTheTotalLength();
    void PrintTheHeaderLength();
    void PrintTheID();
    void PrintTheFragmentInfo();
    void PrintTheTTL();
    void PrintTheProtocol();
    void PrintTheCheckSum();
    void PrintTheSrcIP();
    void PrintTheDstIP();
    void PrintAllIPDetails();
};

#endif //IPCLASS