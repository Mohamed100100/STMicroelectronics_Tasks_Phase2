#ifndef SNIFFER_H
#define SNIFFER_H
// Protocol Type Enum


typedef enum {
    PROTOCOL_IP,
    PROTOCOL_TCP,
    PROTOCOL_UDP,
    PROTOCOL_ICMP,
    PROTOCOL_HTTP,
    PROTOCOL_FTP,
    PROTOCOL_DNS,
    IGNORE              // incase of the filter with ip or port
} protocolType_t;

// Function declarations
protocolType_t GetProtocolType(const unsigned char *packet);


#endif
