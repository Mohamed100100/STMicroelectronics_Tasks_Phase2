// PacketSniffer.cpp
#include "PacketSniffer.h"
#include <QDebug>
#include <thread>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <pcap/pcap.h>
#include "../protocols/HTTPclass.h"
#include "../protocols/DNSclass.h"
#include "../protocols/FTPclass.h"
#include "../protocols/TCPclass.h"
#include "../protocols/UDPclass.h"
#include "../protocols/ICMPclass.h"
#include "../protocols/IPclass.h"

#define ETHERNET_HEADER_LEN 14

QChar* tmppacket ;

PacketSniffer::PacketSniffer(QObject *parent)
    : QObject(parent)
    , handle(nullptr)
    , isRunning(false) {
}

PacketSniffer::~PacketSniffer() {
    stopCapture();
}

QStringList PacketSniffer::getInterfaces() {
    QStringList interfaces;
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        qDebug() << "Error finding devices:" << errbuf;
        return interfaces;
    }

    for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {
        interfaces << QString(d->name);
    }

    pcap_freealldevs(alldevs);
    return interfaces;
}

bool PacketSniffer::startCapture(const std::string& interface,
                                const std::string& ipFilter,
                                uint16_t portFilter) {
    if (isRunning) {
        stopCapture();
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    handle = pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errbuf);

    if (!handle) {
        qDebug() << "Couldn't open device" << interface.c_str() << ":" << errbuf;
        return false;
    }

    // Create struct to pass data to callback
    struct CallbackData {
        PacketSniffer* sniffer;
        std::string ipFilter;
        uint16_t portFilter;
    };

    CallbackData* data = new CallbackData{this, ipFilter, portFilter};

    isRunning = true;
    // Start capture in a separate thread
    std::thread([this, data]() {
        pcap_loop(handle, 0,
            [](u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
                CallbackData* data = reinterpret_cast<CallbackData*>(user);
                data->sniffer->processPacket(packet, data->ipFilter, data->portFilter);
            },
            reinterpret_cast<u_char*>(data));
        delete data;
    }).detach();

    return true;
}

void PacketSniffer::stopCapture() {
    if (isRunning && handle) {
        pcap_breakloop(handle);
        pcap_close(handle);
        handle = nullptr;
        isRunning = false;
    }
}

void PacketSniffer::processPacket(const u_char* packet,
                                 const std::string& ipFilter,
                                 uint16_t portFilter) {
    struct iphdr* ip = (struct iphdr*)(packet + ETHERNET_HEADER_LEN);

    // Convert IP addresses
    char src_ip[INET_ADDRSTRLEN], dest_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip->saddr), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip->daddr), dest_ip, INET_ADDRSTRLEN);

    // Apply IP filter if set
    if (!ipFilter.empty() &&
        ipFilter != src_ip &&
        ipFilter != dest_ip) {
        return;
    }

    QString protocol;
    QString details;

    switch (ip->protocol) {
        case IPPROTO_TCP: {
            struct tcphdr* tcp = (struct tcphdr*)(packet + ETHERNET_HEADER_LEN + ip->ihl * 4);

            // Apply port filter if set
            if (portFilter != 0 &&
                ntohs(tcp->source) != portFilter &&
                ntohs(tcp->dest) != portFilter) {
                return;
            }

            // Check for HTTP
            if (ntohs(tcp->source) == 80 || ntohs(tcp->dest) == 80) {
                httpclass_t httpPacket(packet);
                protocol = "HTTP";
                details = QString("%1|%2|Port %3 → %4")
                    .arg(src_ip)
                    .arg(dest_ip)
                    .arg(ntohs(tcp->source))
                    .arg(ntohs(tcp->dest));
            }
            // Check for FTP
            else if (ntohs(tcp->source) == 21 || ntohs(tcp->dest) == 21) {
                ftpclass_t ftpPacket(packet);
                protocol = "FTP";
                details = QString("%1|%2|Port %3 → %4")
                    .arg(src_ip)
                    .arg(dest_ip)
                    .arg(ntohs(tcp->source))
                    .arg(ntohs(tcp->dest));
            }
            // Regular TCP
            else {
                tcpclass_t tcpPacket(packet);
                protocol = "TCP";
                details = QString("%1|%2|Port %3 → %4")
                    .arg(src_ip)
                    .arg(dest_ip)
                    .arg(ntohs(tcp->source))
                    .arg(ntohs(tcp->dest));
            }
            break;
        }
        case IPPROTO_UDP: {
            struct udphdr* udp = (struct udphdr*)(packet + ETHERNET_HEADER_LEN + ip->ihl * 4);

            // Apply port filter if set
            if (portFilter != 0 &&
                ntohs(udp->source) != portFilter &&
                ntohs(udp->dest) != portFilter) {
                return;
            }

            // Check for DNS
            if (ntohs(udp->source) == 53 || ntohs(udp->dest) == 53) {
                dnsclass_t dnsPacket(packet);
                protocol = "DNS";
                details = QString("%1|%2|Port %3 → %4")
                    .arg(src_ip)
                    .arg(dest_ip)
                    .arg(ntohs(udp->source))
                    .arg(ntohs(udp->dest));
            }
            // Regular UDP
            else {
                udpclass_t udpPacket(packet);
                protocol = "UDP";
                details = QString("%1|%2|Port %3 → %4")
                    .arg(src_ip)
                    .arg(dest_ip)
                    .arg(ntohs(udp->source))
                    .arg(ntohs(udp->dest));
            }
            break;
        }
        case IPPROTO_ICMP: {
            if (portFilter != 0) return;

            icmpclass_t icmpPacket(packet);
            protocol = "ICMP";
            details = QString("%1|%2|ICMP Message").arg(src_ip).arg(dest_ip);
            break;
        }
        default: {
            if (portFilter != 0) return;

            ipclass_t ipPacket(packet);
            protocol = "IP";
            details = QString("%1|%2|Protocol: %3")
                .arg(src_ip)
                .arg(dest_ip)
                .arg(ip->protocol);
            break;
        }
    }

    // When emitting the signal, include the packet length
    emit packetCaptured(protocol, details, packet, ntohs(ip->tot_len));
}
