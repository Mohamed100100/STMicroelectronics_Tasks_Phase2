#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "HTTPclass.h"

void httpclass_t::PrintAllHTTPDetails() {
    fwrite(this->data_start, 1, this->httpDataLength, stdout);
}

httpclass_t::httpclass_t(){}
httpclass_t::httpclass_t( const unsigned char *packet):tcpclass_t(packet) {

    struct iphdr *ip_header = (struct iphdr *)(packet + 14); 
    struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + (ip_header->ihl)*4);  

    this->data_start = packet + 14 + (ip_header->ihl)*4 + tcp_header->doff*4;

    uint16_t http_data_length = ntohs(ip_header->tot_len) - (ip_header->ihl* 4) - (tcp_header->doff * 4);
    
    this->httpDataLength = http_data_length;
}


