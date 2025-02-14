// SplashScreen.h
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

class SplashScreen : public QDialog {
    Q_OBJECT

public:
    SplashScreen(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        QVBoxLayout *layout = new QVBoxLayout(this);

        // Logo
        QLabel *logoLabel = new QLabel(this);
        QPixmap logo("/home/gemy/Desktop/qt/wirefish/src/images/wirefish_logo.png");  // Add your logo to resources

        logoLabel->setPixmap(logo.scaled(400, 400, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        layout->addWidget(logoLabel, 0, Qt::AlignBottom);

        // Author
        QLabel *authorLabel = new QLabel("Created by GEMY", this);
        authorLabel->setStyleSheet("font-size: 12pt; color: #2196F3;");
        layout->addWidget(authorLabel, 0, Qt::AlignCenter);

        // Auto-close after 3 seconds
        QTimer::singleShot(5000, this, &QDialog::accept);
    }
};

#endif
