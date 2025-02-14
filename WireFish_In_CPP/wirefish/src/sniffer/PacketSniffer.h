// PacketSniffer.h
#ifndef PACKETSNIFFER_H
#define PACKETSNIFFER_H

#include <QObject>
#include <pcap.h>
#include <string>

class PacketSniffer : public QObject {
    Q_OBJECT

public:
    PacketSniffer(QObject *parent = nullptr);
    ~PacketSniffer();

    bool startCapture(const std::string& interface,
                     const std::string& ipFilter = "",
                     uint16_t portFilter = 0);
    void stopCapture();
    static QStringList getInterfaces();

signals:
    void packetCaptured(const QString& protocol, const QString& details, const u_char* packet, qint64 length);

private:
    void processPacket(const u_char* packet,
                      const std::string& ipFilter,
                      uint16_t portFilter);
    pcap_t *handle;
    bool isRunning;
};

#endif
