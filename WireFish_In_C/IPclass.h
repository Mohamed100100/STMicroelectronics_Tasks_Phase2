#ifndef _IPCLASS_H
#define _IPCLASS_H

#include <stdint.h>

typedef struct ipclass ipclass_t;

struct ipclass{
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

    void (*PrintTheVersion)(ipclass_t *IPObj);
    void (*PrintTheTotalLength)(ipclass_t *IPObj);
    void (*PrintTheHeaderLength)(ipclass_t *IPObj);
    void (*PrintTheID)(ipclass_t *IPObj);
    void (*PrintTheFragmentInfo)(ipclass_t *IPObj);
    void (*PrintTheTTL)(ipclass_t *IPObj);
    void (*PrintTheProtocol)(ipclass_t *IPObj);
    void (*PrintTheCheckSum)(ipclass_t *IPObj);
    void (*PrintTheSrcIP)(ipclass_t *IPObj);
    void (*PrintTheDstIP)(ipclass_t *IPObj);

    // method to call all print functions
    void (*PrintAllIPDetails)(ipclass_t *IPObj);
};

ipclass_t* ConstructIPObject(const  unsigned char *packet);

void DeconstructIPObject(ipclass_t *ipclassObj);

#endif //IPCLASS