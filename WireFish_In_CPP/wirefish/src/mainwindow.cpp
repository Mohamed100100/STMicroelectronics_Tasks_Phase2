#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QTime>
#include <QDebug>

#include <pcap/pcap.h>
#include <netinet/in.h> // For byte order functions (ntohs)
#include <arpa/inet.h>  // For IP address functions
#include <netinet/ip.h>


#include "PacketDetailsDialog.h"
#include "src/protocols/HTTPclass.h"
#include "src/protocols/DNSclass.h"
#include "src/protocols/FTPclass.h"
#include "src/protocols/ICMPclass.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , sniffer(new PacketSniffer(this))
    , updateTimer(new QTimer(this))
{
    setupUI();
    setupConnections();
    updateInterfaces();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Control panel
    QGridLayout *controlLayout = new QGridLayout();

    // Interface selection
    QLabel *interfaceLabel = new QLabel("Interface:", this);
    interfaceCombo = new QComboBox(this);
    controlLayout->addWidget(interfaceLabel, 0, 0);
    controlLayout->addWidget(interfaceCombo, 0, 1);

    // IP filter
    QLabel *ipLabel = new QLabel("IP Filter:", this);
    ipFilterEdit = new QLineEdit(this);
    ipFilterEdit->setPlaceholderText("Enter IP to filter");
    controlLayout->addWidget(ipLabel, 0, 2);
    controlLayout->addWidget(ipFilterEdit, 0, 3);

    // Port filter
    QLabel *portLabel = new QLabel("Port Filter:", this);
    portFilterEdit = new QLineEdit(this);
    portFilterEdit->setPlaceholderText("Enter port to filter");
    controlLayout->addWidget(portLabel, 0, 4);
    controlLayout->addWidget(portFilterEdit, 0, 5);

    // Control buttons
    startButton = new QPushButton("Start Capture", this);
    stopButton = new QPushButton("Stop Capture", this);
    stopButton->setEnabled(false);
    controlLayout->addWidget(startButton, 1, 0);
    controlLayout->addWidget(stopButton, 1, 1);

    mainLayout->addLayout(controlLayout);

    // Add clear button
    QPushButton *clearButton = new QPushButton("Clear Packets", this);
    controlLayout->addWidget(clearButton, 1, 2);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearPackets);

    // Packet table
    packetTable = new QTableWidget(this);
    packetTable->setColumnCount(5);
    packetTable->setHorizontalHeaderLabels(
                {"Time", "Protocol", "Source", "Destination", "Details"}
                );
    packetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(packetTable);

    setCentralWidget(centralWidget);
    setWindowTitle("WireFish Packet Sniffer");
    resize(1200, 800);


    // Connect table double click
    connect(packetTable, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::showPacketDetails);

}

void MainWindow::setupConnections() {
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSniffing);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSniffing);
    connect(sniffer, &PacketSniffer::packetCaptured,
            this, &MainWindow::handlePacket);  // Make sure this matches
    connect(updateTimer, &QTimer::timeout,
            this, &MainWindow::updateInterfaces);

    updateTimer->start(5000);
}
void MainWindow::startSniffing() {
    QString interface = interfaceCombo->currentText();
    QString ipFilter = ipFilterEdit->text();
    uint16_t portFilter = portFilterEdit->text().toUShort();

    if (sniffer->startCapture(interface.toStdString(),
                              ipFilter.toStdString(),
                              portFilter)) {
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        interfaceCombo->setEnabled(false);
        ipFilterEdit->setEnabled(false);
        portFilterEdit->setEnabled(false);
    } else {
        QMessageBox::critical(this, "Error",
                              "Failed to start packet capture. Check permissions.");
    }
}

void MainWindow::clearPackets() {
    packetTable->setRowCount(0);
    packetData.clear();
}

void MainWindow::stopSniffing() {
    sniffer->stopCapture();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    interfaceCombo->setEnabled(true);
    ipFilterEdit->setEnabled(true);
    portFilterEdit->setEnabled(true);
}

void MainWindow::updateInterfaces() {
    QString currentInterface = interfaceCombo->currentText();
    interfaceCombo->clear();
    interfaceCombo->addItems(PacketSniffer::getInterfaces());

    // Restore previous selection if still available
    int index = interfaceCombo->findText(currentInterface);
    if (index >= 0) {
        interfaceCombo->setCurrentIndex(index);
    }
}

void MainWindow::showPacketDetails(int row, int column) {
    QString protocol = packetTable->item(row, 1)->text();
    QString details = packetTable->item(row, 4)->text();
    qDebug()<< protocol<<details<<"Iam here\n";

    // Get the stored packet data
    const u_char* packet = packetData[row].data.data();    QString fullDetails;
    qDebug()<< "Iam here\n";


    if (protocol == "HTTP") {
        httpclass_t httpObj(packet);
        fullDetails += "<h3>IP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Version: %1\n").arg(httpObj.version);
        fullDetails += QString("Header Length: %1 words\n").arg(httpObj.ihl);
        fullDetails += QString("Type of Service: %1\n").arg(httpObj.tos);
        fullDetails += QString("Total Length: %1 bytes\n").arg(httpObj.tot_len);
        fullDetails += QString("ID: %1\n").arg(httpObj.id);
        fullDetails += QString("Fragment Offset: %1\n").arg(httpObj.frag_off);
        fullDetails += QString("TTL: %1\n").arg(httpObj.ttl);
        fullDetails += QString("Protocol: %1\n").arg(httpObj.protocol);
        fullDetails += QString("Checksum: 0x%1\n").arg(httpObj.checksum, 4, 16, QChar('0'));

        // Convert IP addresses to string format
        char buffer[INET_ADDRSTRLEN];  // Buffer to store the IP string
        struct in_addr addr;
        addr.s_addr = httpObj.src_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Source IP: %1\n").arg(QString(buffer));
        addr.s_addr = httpObj.dest_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Destination IP: %1\n").arg(QString(buffer));
        fullDetails += "</pre>";

        fullDetails += "<h3>TCP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Source Port: %1\n").arg(httpObj.source);
        fullDetails += QString("Destination Port: %1\n").arg(httpObj.dest);
        fullDetails += QString("Sequence Number: %1\n").arg(httpObj.seq);
        fullDetails += QString("Acknowledgment Number: %1\n").arg(httpObj.ack_seq);
        fullDetails += QString("Header Length: %1 bytes\n").arg(httpObj.offset * 4);

        // TCP Flags
        fullDetails += "Flags:\n";
        fullDetails += QString("  URG: %1\n").arg(httpObj.urg ? "Yes" : "No");
        fullDetails += QString("  ACK: %1\n").arg(httpObj.ack ? "Yes" : "No");
        fullDetails += QString("  PSH: %1\n").arg(httpObj.psh ? "Yes" : "No");
        fullDetails += QString("  RST: %1\n").arg(httpObj.rst ? "Yes" : "No");
        fullDetails += QString("  SYN: %1\n").arg(httpObj.syn ? "Yes" : "No");
        fullDetails += QString("  FIN: %1\n").arg(httpObj.fin ? "Yes" : "No");

        fullDetails += QString("Window Size: %1\n").arg(httpObj.window);
        fullDetails += QString("Checksum: 0x%1\n").arg(httpObj.check, 4, 16, QChar('0'));
        fullDetails += QString("Urgent Pointer: %1\n").arg(httpObj.urg_ptr);
        fullDetails += "</pre>";

        fullDetails += "<h3>HTTP Details</h3>";
        fullDetails += "<pre>";
        // Convert HTTP data to string and escape HTML special characters
        QByteArray httpData(reinterpret_cast<const char*>(httpObj.data_start), httpObj.httpDataLength);
        QString httpText = QString(httpData).toHtmlEscaped();
        fullDetails += httpText;
        fullDetails += "</pre>";
    }
   else if (protocol == "DNS") {
        dnsclass_t dnsObj(packet);
        fullDetails += "<h3>IP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Version: %1\n").arg(dnsObj.version);
        fullDetails += QString("Header Length: %1 words\n").arg(dnsObj.ihl);
        fullDetails += QString("Type of Service: %1\n").arg(dnsObj.tos);
        fullDetails += QString("Total Length: %1 bytes\n").arg(dnsObj.tot_len);
        fullDetails += QString("ID: %1\n").arg(dnsObj.id);
        fullDetails += QString("Fragment Offset: %1\n").arg(dnsObj.frag_off);
        fullDetails += QString("TTL: %1\n").arg(dnsObj.ttl);
        fullDetails += QString("Protocol: %1\n").arg(dnsObj.protocol);
        fullDetails += QString("Checksum: 0x%1\n").arg(dnsObj.checksum, 4, 16, QChar('0'));

        // Convert IP addresses
        char buffer[INET_ADDRSTRLEN];
        struct in_addr addr;
        addr.s_addr = dnsObj.src_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Source IP: %1\n").arg(QString(buffer));
        addr.s_addr = dnsObj.dest_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Destination IP: %1\n").arg(QString(buffer));
        fullDetails += "</pre>";

        fullDetails += "<h3>UDP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Source Port: %1\n").arg(dnsObj.source);
        fullDetails += QString("Destination Port: %1\n").arg(dnsObj.dest);
        fullDetails += QString("Datagram Length: %1 bytes\n").arg(dnsObj.len);
        fullDetails += QString("Checksum: 0x%1\n").arg(dnsObj.check, 4, 16, QChar('0'));
        fullDetails += "</pre>";

        fullDetails += "<h3>DNS Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Transaction ID: 0x%1\n").arg(dnsObj.transaction_id, 4, 16, QChar('0'));
        fullDetails += QString("Flags: 0x%1\n").arg(dnsObj.flags, 4, 16, QChar('0'));
        fullDetails += QString("Questions: %1\n").arg(dnsObj.questions);
        fullDetails += QString("Answers: %1\n").arg(dnsObj.answers);
        fullDetails += QString("Authority Records: %1\n").arg(dnsObj.authority_rr);
        fullDetails += QString("Additional Records: %1\n").arg(dnsObj.additional_rr);
        fullDetails += "</pre>";

    }else if (protocol == "FTP") {
        ftpclass_t ftpObj(packet);
        fullDetails += "<h3>IP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Version: %1\n").arg(ftpObj.version);
        fullDetails += QString("Header Length: %1 words\n").arg(ftpObj.ihl);
        fullDetails += QString("Type of Service: %1\n").arg(ftpObj.tos);
        fullDetails += QString("Total Length: %1 bytes\n").arg(ftpObj.tot_len);
        fullDetails += QString("ID: %1\n").arg(ftpObj.id);
        fullDetails += QString("Fragment Offset: %1\n").arg(ftpObj.frag_off);
        fullDetails += QString("TTL: %1\n").arg(ftpObj.ttl);
        fullDetails += QString("Protocol: %1\n").arg(ftpObj.protocol);
        fullDetails += QString("Checksum: 0x%1\n").arg(ftpObj.checksum, 4, 16, QChar('0'));

        // Convert IP addresses
        char buffer[INET_ADDRSTRLEN];
        struct in_addr addr;
        addr.s_addr = ftpObj.src_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Source IP: %1\n").arg(QString(buffer));
        addr.s_addr = ftpObj.dest_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Destination IP: %1\n").arg(QString(buffer));
        fullDetails += "</pre>";

        fullDetails += "<h3>TCP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Source Port: %1\n").arg(ftpObj.source);
        fullDetails += QString("Destination Port: %1\n").arg(ftpObj.dest);
        fullDetails += QString("Sequence Number: %1\n").arg(ftpObj.seq);
        fullDetails += QString("Acknowledgment Number: %1\n").arg(ftpObj.ack_seq);
        fullDetails += QString("Header Length: %1 bytes\n").arg(ftpObj.offset * 4);

        // TCP Flags
        fullDetails += "Flags:\n";
        fullDetails += QString("  URG: %1\n").arg(ftpObj.urg ? "Yes" : "No");
        fullDetails += QString("  ACK: %1\n").arg(ftpObj.ack ? "Yes" : "No");
        fullDetails += QString("  PSH: %1\n").arg(ftpObj.psh ? "Yes" : "No");
        fullDetails += QString("  RST: %1\n").arg(ftpObj.rst ? "Yes" : "No");
        fullDetails += QString("  SYN: %1\n").arg(ftpObj.syn ? "Yes" : "No");
        fullDetails += QString("  FIN: %1\n").arg(ftpObj.fin ? "Yes" : "No");

        fullDetails += QString("Window Size: %1\n").arg(ftpObj.window);
        fullDetails += QString("Checksum: 0x%1\n").arg(ftpObj.check, 4, 16, QChar('0'));
        fullDetails += QString("Urgent Pointer: %1\n").arg(ftpObj.urg_ptr);
        fullDetails += "</pre>";

        fullDetails += "<h3>FTP Data</h3>";
        fullDetails += "<pre>";
        QByteArray ftpData(reinterpret_cast<const char*>(ftpObj.data_start), ftpObj.ftpDataLength);
        QString ftpText = QString(ftpData).toHtmlEscaped();
        fullDetails += ftpText;
        fullDetails += "</pre>";
    }else if (protocol == "IP") {
        ipclass_t ipObj(packet);
        fullDetails += "<h3>IP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Version: %1\n").arg(ipObj.version);
        fullDetails += QString("Header Length: %1 words\n").arg(ipObj.ihl);
        fullDetails += QString("Type of Service: %1\n").arg(ipObj.tos);
        fullDetails += QString("Total Length: %1 bytes\n").arg(ipObj.tot_len);
        fullDetails += QString("ID: %1\n").arg(ipObj.id);
        fullDetails += QString("Fragment Offset: %1\n").arg(ipObj.frag_off);
        fullDetails += QString("TTL: %1\n").arg(ipObj.ttl);
        fullDetails += QString("Protocol: %1\n").arg(ipObj.protocol);
        fullDetails += QString("Checksum: 0x%1\n").arg(ipObj.checksum, 4, 16, QChar('0'));

        // Convert IP addresses
        char buffer[INET_ADDRSTRLEN];
        struct in_addr addr;
        addr.s_addr = ipObj.src_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Source IP: %1\n").arg(QString(buffer));
        addr.s_addr = ipObj.dest_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Destination IP: %1\n").arg(QString(buffer));
        fullDetails += "</pre>";
    }else if (protocol == "UDP") {
        udpclass_t udpObj(packet);
        fullDetails += "<h3>IP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Version: %1\n").arg(udpObj.version);
        fullDetails += QString("Header Length: %1 words\n").arg(udpObj.ihl);
        fullDetails += QString("Type of Service: %1\n").arg(udpObj.tos);
        fullDetails += QString("Total Length: %1 bytes\n").arg(udpObj.tot_len);
        fullDetails += QString("ID: %1\n").arg(udpObj.id);
        fullDetails += QString("Fragment Offset: %1\n").arg(udpObj.frag_off);
        fullDetails += QString("TTL: %1\n").arg(udpObj.ttl);
        fullDetails += QString("Protocol: %1\n").arg(udpObj.protocol);
        fullDetails += QString("Checksum: 0x%1\n").arg(udpObj.checksum, 4, 16, QChar('0'));

        // Convert IP addresses
        char buffer[INET_ADDRSTRLEN];
        struct in_addr addr;
        addr.s_addr = udpObj.src_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Source IP: %1\n").arg(QString(buffer));
        addr.s_addr = udpObj.dest_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Destination IP: %1\n").arg(QString(buffer));
        fullDetails += "</pre>";

        fullDetails += "<h3>UDP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Source Port: %1\n").arg(udpObj.source);
        fullDetails += QString("Destination Port: %1\n").arg(udpObj.dest);
        fullDetails += QString("Length: %1 bytes\n").arg(udpObj.len);
        fullDetails += QString("Checksum: 0x%1\n").arg(udpObj.check, 4, 16, QChar('0'));
        fullDetails += "</pre>";
    }else if (protocol == "TCP") {
        tcpclass_t tcpObj(packet);
        fullDetails += "<h3>IP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Version: %1\n").arg(tcpObj.version);
        fullDetails += QString("Header Length: %1 words\n").arg(tcpObj.ihl);
        fullDetails += QString("Type of Service: %1\n").arg(tcpObj.tos);
        fullDetails += QString("Total Length: %1 bytes\n").arg(tcpObj.tot_len);
        fullDetails += QString("ID: %1\n").arg(tcpObj.id);
        fullDetails += QString("Fragment Offset: %1\n").arg(tcpObj.frag_off);
        fullDetails += QString("TTL: %1\n").arg(tcpObj.ttl);
        fullDetails += QString("Protocol: %1\n").arg(tcpObj.protocol);
        fullDetails += QString("Checksum: 0x%1\n").arg(tcpObj.checksum, 4, 16, QChar('0'));

        // Convert IP addresses
        char buffer[INET_ADDRSTRLEN];
        struct in_addr addr;
        addr.s_addr = tcpObj.src_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Source IP: %1\n").arg(QString(buffer));
        addr.s_addr = tcpObj.dest_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Destination IP: %1\n").arg(QString(buffer));
        fullDetails += "</pre>";

        fullDetails += "<h3>TCP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Source Port: %1\n").arg(tcpObj.source);
        fullDetails += QString("Destination Port: %1\n").arg(tcpObj.dest);
        fullDetails += QString("Sequence Number: %1\n").arg(tcpObj.seq);
        fullDetails += QString("Acknowledgment Number: %1\n").arg(tcpObj.ack_seq);
        fullDetails += QString("Header Length: %1 bytes\n").arg(tcpObj.offset * 4);

        // TCP Flags
        fullDetails += "Flags:\n";
        fullDetails += QString("  URG: %1\n").arg(tcpObj.urg ? "Yes" : "No");
        fullDetails += QString("  ACK: %1\n").arg(tcpObj.ack ? "Yes" : "No");
        fullDetails += QString("  PSH: %1\n").arg(tcpObj.psh ? "Yes" : "No");
        fullDetails += QString("  RST: %1\n").arg(tcpObj.rst ? "Yes" : "No");
        fullDetails += QString("  SYN: %1\n").arg(tcpObj.syn ? "Yes" : "No");
        fullDetails += QString("  FIN: %1\n").arg(tcpObj.fin ? "Yes" : "No");

        fullDetails += QString("Window Size: %1\n").arg(tcpObj.window);
        fullDetails += QString("Checksum: 0x%1\n").arg(tcpObj.check, 4, 16, QChar('0'));
        fullDetails += QString("Urgent Pointer: %1\n").arg(tcpObj.urg_ptr);
        fullDetails += "</pre>";
    }else if (protocol == "ICMP") {
        icmpclass_t icmpObj(packet);

        fullDetails += "<h3>IP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("Version: %1\n").arg(icmpObj.version);
        fullDetails += QString("Header Length: %1 words\n").arg(icmpObj.ihl);
        fullDetails += QString("Type of Service: %1\n").arg(icmpObj.tos);
        fullDetails += QString("Total Length: %1 bytes\n").arg(icmpObj.tot_len);
        fullDetails += QString("ID: %1\n").arg(icmpObj.id);
        fullDetails += QString("Fragment Offset: %1\n").arg(icmpObj.frag_off);
        fullDetails += QString("TTL: %1\n").arg(icmpObj.ttl);
        fullDetails += QString("Protocol: %1 (ICMP)\n").arg(icmpObj.protocol);
        fullDetails += QString("Checksum: 0x%1\n").arg(icmpObj.checksum, 4, 16, QChar('0'));

        // Convert IP addresses to string format
        char buffer[INET_ADDRSTRLEN];
        struct in_addr addr;

        addr.s_addr = icmpObj.src_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Source IP: %1\n").arg(QString(buffer));

        addr.s_addr = icmpObj.dest_ip;
        inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
        fullDetails += QString("Destination IP: %1\n").arg(QString(buffer));

        fullDetails += "</pre>";

        fullDetails += "<h3>ICMP Header Details</h3>";
        fullDetails += "<pre>";
        fullDetails += QString("ICMP Type: %1\n").arg(icmpObj.type);
        fullDetails += QString("ICMP Code: %1\n").arg(icmpObj.code);
        fullDetails += QString("Checksum: 0x%1\n").arg(icmpObj.checksum, 4, 16, QChar('0'));
        fullDetails += QString("Identifier: %1\n").arg(icmpObj.identifier);
        fullDetails += QString("Sequence Number: %1\n").arg(icmpObj.sequence);
        fullDetails += "</pre>";
    }


    PacketDetailsDialog *dialog = new PacketDetailsDialog(fullDetails, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::handlePacket(const QString& protocol, const QString& details,const u_char* packet,qint64 length) {
    int row = packetTable->rowCount();
    packetTable->insertRow(row);
    packetData.append(StoredPacket(packet, length));
    // Store the packet data
    // Parse details string
    QStringList detailsList = details.split("|");
    if (detailsList.size() >= 3) {
        packetTable->setItem(row, 0,
                             new QTableWidgetItem(QTime::currentTime().toString()));
        packetTable->setItem(row, 1,
                             new QTableWidgetItem(protocol));
        packetTable->setItem(row, 2,
                             new QTableWidgetItem(detailsList[0]));
        packetTable->setItem(row, 3,
                             new QTableWidgetItem(detailsList[1]));
        packetTable->setItem(row, 4,
                             new QTableWidgetItem(detailsList[2]));
    }

    packetTable->scrollToBottom();
}

MainWindow::~MainWindow() {
    stopSniffing();
}

#endif
