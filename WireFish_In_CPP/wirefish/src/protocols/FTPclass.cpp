#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "FTPclass.h"

void ftpclass_t::PrintAllFTPDetails( ) {

    fwrite(this->data_start, 1, this->ftpDataLength, stdout);
}

ftpclass_t::ftpclass_t(){}
ftpclass_t::ftpclass_t(const unsigned char *packet):tcpclass_t(packet) {

    struct iphdr *ip_header = (struct iphdr *)(packet + 14); 
    struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + (ip_header->ihl)*4);  

    uint16_t data_length = ntohs(ip_header->tot_len) - (ip_header->ihl* 4) - (tcp_header->doff * 4);

    // Pointer to the start of FTP data in the packet
    this->data_start = packet + 14 + (ip_header->ihl) * 4 + tcp_header->doff*4;

    this->ftpDataLength = data_length;

}


