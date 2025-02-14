// PacketDetailsDialog.h
#ifndef PACKETDETAILSDIALOG_H
#define PACKETDETAILSDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>

class PacketDetailsDialog : public QDialog {
    Q_OBJECT

public:
    PacketDetailsDialog(const QString& details, QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Packet Details");
        resize(600, 400);

        QVBoxLayout *layout = new QVBoxLayout(this);
        QTextEdit *textEdit = new QTextEdit(this);
        textEdit->setReadOnly(true);
        textEdit->setHtml(details);
        layout->addWidget(textEdit);
    }
};

#endif
