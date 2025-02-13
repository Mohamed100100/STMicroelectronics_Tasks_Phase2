#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "HTTPclass.h"
#include "DNSclass.h"
#include "FTPclass.h"
#include "TCPclass.h"
#include "UDPclass.h"
#include "IPclass.h"
#include "ICMPclass.h"
#include "sniffer.h"

#define ETHERNET_HEADER_LEN 14

char *filter_ip = NULL;     // Store the target IP to filter
uint16_t filter_port = 0;   // Store the target Port to filter
char interface[50];         // Store interface name
pcap_t *handle = NULL;      // Global pcap handle

void start_sniffing();  // Function to restart sniffing

// Handle CTRL+C signal to pause and display menu
void handle_signal(int sig) {
        pcap_close(handle);
        handle = NULL;
}
void editTheFilters(){
    printf("\n\n\033[33m==================================\033[0m\n");
    printf("\033[33m========= Sniffer Paused =========\033[0m\n");
    printf("\033[33m==================================\033[0m\n");


while (1)
{
    printf("Current Filters:\n");
    printf("Interface: %s\n", interface);
    if (filter_ip) printf("IP Filter: %s\n", filter_ip);
    else printf("IP Filter: None\n");
    
    if (filter_port) printf("Port Filter: %d\n", filter_port);
    else printf("Port Filter: None\n");

    printf("\nOptions:\n");
    printf("1) Edit IP filter\n");
    printf("2) Edit Port filter\n");
    printf("3) Edit Interface\n");
    printf("4) Reset filters\n");
    printf("5) Resume sniffing\n");
    printf("6) Exit\n");
    
    printf("\nEnter choice: ");
    int choice;
    scanf("%d", &choice);
    getchar();  // Consume newline

    switch (choice) {
        case 1:
            printf("Enter new IP filter: ");
            filter_ip = malloc(20);
            scanf("%19s", filter_ip);
            printf("\n");
            break;
        case 2:
            printf("Enter new Port filter: ");
            scanf("%hu", &filter_port);
            printf("\n");
            break;
        case 3:
            printf("Enter new Interface: ");
            scanf("%49s", interface);
            printf("\n");
            break;
        case 4:
            filter_ip = NULL;
            filter_port = 0;
            printf("Filters reset!\n\n");
            break;
        case 5:
            printf("\n\n\033[33m==================================\033[0m\n");
            printf("\033[33m======= Resuming Sniffing ========\033[0m\n");
            printf("\033[33m==================================\033[0m\n");
            return;
        case 6:
            printf("Exiting...\n\n");
            exit(0);
        default:
            printf("Invalid choice!\n\n");
    }
}
}
// Print packet details based on protocol type
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    protocolType_t protocol = GetProtocolType(packet);
    
    if(protocol == IGNORE) return;
    printf("Packet captured:\n");
    switch (protocol) {
        case PROTOCOL_HTTP: {
            httpclass_t *httpObj = ConstructHTTPObject(packet);

            printf("\033[31mProtocol: IP \033[0m\n");
            httpObj->tcpPart->ipPart->PrintAllIPDetails(httpObj->tcpPart->ipPart);
            
            
            printf("\033[31mProtocol: TCP \033[0m\n");    
            httpObj->tcpPart->PrintAllTCPDetails(httpObj->tcpPart);

            printf("\033[31mProtocol: HTTP \033[0m\n");
            httpObj->PrintAllHTTPDetails(httpObj);

            DeconstructHTTPObject(httpObj);
            break;
        }
        case PROTOCOL_FTP: {
            ftpclass_t *ftpObj = ConstructFTPObject(packet);

            printf("\033[31mProtocol: IP \033[0m\n");
            ftpObj->tcpPart->ipPart->PrintAllIPDetails(ftpObj->tcpPart->ipPart);
            
            printf("\033[31mProtocol: TCP \033[0m\n");    
            ftpObj->tcpPart->PrintAllTCPDetails(ftpObj->tcpPart);

            printf("\033[31mProtocol: FTP \033[0m\n");
            ftpObj->PrintAllFTPDetails(ftpObj);

            DeconstructFTPObject(ftpObj);
            break;

        case PROTOCOL_DNS: {
            dnsclass_t *dnsObj = ConstructDNSObject(packet);

            printf("\033[31mProtocol: IP \033[0m\n");
            dnsObj->udpPart->ipPart->PrintAllIPDetails(dnsObj->udpPart->ipPart);

            printf("\033[31mProtocol: UDP \033[0m\n");
            dnsObj->udpPart->PrintAllUDPDetails(dnsObj->udpPart);

            printf("\033[31mProtocol: DNS \033[0m\n");
            dnsObj->PrintAllDNSDetails(dnsObj);

            DeconstructDNSObject(dnsObj);
            break;
        }
        case PROTOCOL_UDP: {
            udpclass_t *udpObj = ConstructUDPObject(packet);

            printf("\033[31mProtocol: IP \033[0m\n");
            udpObj->ipPart->PrintAllIPDetails(udpObj->ipPart);

            printf("\033[31mProtocol: UDP \033[0m\n");
            udpObj->PrintAllUDPDetails(udpObj);

            DeconstructUDPObject(udpObj);
            break;
        }
        case PROTOCOL_ICMP: {
            icmpclass_t *icmpObj = ConstructICMPObject(packet);

            printf("\033[31mProtocol: IP \033[0m\n");
            icmpObj->ipPart->PrintAllIPDetails(icmpObj->ipPart);
          
            printf("\033[31mProtocol: ICMP \033[0m\n");
            icmpObj->PrintAllICMPDetails(icmpObj);
            
            DeconstructICMPObject(icmpObj);
            break;
        }
        case PROTOCOL_TCP: {

            tcpclass_t *tcpObj = ConstructTCPObject(packet);

            printf("\033[31mProtocol: IP \033[0m\n");
            tcpObj->ipPart->PrintAllIPDetails(tcpObj->ipPart);
          
            printf("\033[31mProtocol: TCP \033[0m\n");
            tcpObj->PrintAllTCPDetails(tcpObj);
            
            DeconstructTCPObject(tcpObj);
            break;
        }
        case PROTOCOL_IP:{
            printf("\033[31mProtocol: IP \033[0m\n");
            ipclass_t *ipObj = ConstructIPObject(packet);
            ipObj->PrintAllIPDetails(ipObj);
            DeconstructIPObject(ipObj);
            break;
        }
        default:
            printf("Protocol: IP or Unknown\n");
            break;
    }
}
    printf("\n");

}

// Get protocol type based on packet data
protocolType_t GetProtocolType(const unsigned char *packet) {
    struct iphdr *ip = (struct iphdr*)(packet + ETHERNET_HEADER_LEN);

    // Convert source and destination IP to readable format
    char src_ip[INET_ADDRSTRLEN], dest_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip->saddr), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip->daddr), dest_ip, INET_ADDRSTRLEN);

    // Extract transport layer headers (TCP/UDP)
    // Apply Filtering
    if (filter_ip && (strcmp(src_ip, filter_ip) != 0 && strcmp(dest_ip, filter_ip) != 0)) {
        return(IGNORE);  // Ignore packet if IP doesn't match
    }

    switch (ip->protocol) {
        case IPPROTO_TCP: {
            struct tcphdr *tcp = (struct tcphdr*)(packet + ETHERNET_HEADER_LEN + ip->ihl * 4);

            if (filter_port && (ntohs(tcp->source) != filter_port &&  (ntohs(tcp->dest) != filter_port))) {
            return(IGNORE);  // Drop packet if port doesn't match
            }

            if (ntohs(tcp->source) == 80 || ntohs(tcp->dest) == 80) {
                return PROTOCOL_HTTP;
            }
            if (ntohs(tcp->source) == 21 || ntohs(tcp->dest) == 21) {
                return PROTOCOL_FTP;
            }
            // otherwise
            return (PROTOCOL_TCP);
            break;
        }
        case IPPROTO_UDP: {
            struct udphdr *udp = (struct udphdr*)(packet + ETHERNET_HEADER_LEN + ip->ihl * 4);

            if (filter_port && (ntohs(udp->source) != filter_port &&  (ntohs(udp->dest) != filter_port))) {
                return(IGNORE);  // Drop packet if port doesn't match
            }
            if (ntohs(udp->source) == 53 || ntohs(udp->dest) == 53) {
                return PROTOCOL_DNS;
            }
            // Not DNS
            return PROTOCOL_UDP;
        }
        case IPPROTO_ICMP:
              if (filter_port) return IGNORE;
    
            return PROTOCOL_ICMP;
        default:
            if (filter_port) return IGNORE;
            return PROTOCOL_IP;
    }
}


// Start packet capture with updated filters
void start_sniffing() {
    char errbuf[PCAP_ERRBUF_SIZE];
    if (handle) pcap_close(handle);  // Close previous handle

    handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
    if (!handle) {
        fprintf(stderr, "Couldn't open device %s: %s\n", interface, errbuf);
        exit(1);
    }

    printf("Sniffing on interface: %s\n", interface);
    if (filter_ip) printf("- Filtering IP: %s\n", filter_ip);
    if (filter_port) printf("- Filtering Port: %d\n", filter_port);

    pcap_loop(handle, 0, packet_handler, NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <interface> [-ip <IP>] [-port <PORT>]\n", argv[0]);
        return 1;
    }

    strcpy(interface, argv[1]);

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-ip") == 0) filter_ip = argv[++i];
        else if (strcmp(argv[i], "-port") == 0) filter_port = atoi(argv[++i]);
        else {
            printf("Error: Unknown option %s\n", argv[i]);
            return 1;
        }
    }

    signal(SIGINT, handle_signal);  // Catch Ctrl+C
    printf("\033[33m==================================\033[0m\n");
    printf("\033[33m======= Starting Sniffing ========\033[0m\n");
    printf("\033[33m==================================\033[0m\n");
    while(1){
        start_sniffing();  // Start packet capture
        editTheFilters();
    }
    return 0;
}
