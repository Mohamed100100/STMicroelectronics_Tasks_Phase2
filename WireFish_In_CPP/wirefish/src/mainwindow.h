#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include "sniffer/PacketSniffer.h"

struct StoredPacket {
    std::vector<u_char> data;
    explicit StoredPacket(const u_char* packet, qint64 length) {
        data.assign(packet, packet + length);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startSniffing();
    void stopSniffing();
    void clearPackets();
//    void updateFilters();
    void handlePacket(const QString& protocol, const QString& details,const u_char* packet,qint64 length);  // Changed from packetCaptured
    void updateInterfaces();
    void showPacketDetails(int row, int column);

private:
    void setupUI();
    void setupConnections();
    QString capturePacketDetails(const QString& protocol, const u_char* packet);


    QVector<StoredPacket> packetData;  // Instead of QVector<const u_char*>
    QComboBox *interfaceCombo;
    QLineEdit *ipFilterEdit;
    QLineEdit *portFilterEdit;
    QPushButton *startButton;
    QPushButton *stopButton;
    QTableWidget *packetTable;
    PacketSniffer *sniffer;
    QTimer *updateTimer;
};
