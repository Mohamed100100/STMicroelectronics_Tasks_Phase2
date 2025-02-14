#include "mainwindow.h"

#include <QApplication>
#include "SplashScreen.h"
//In main.cpp:
int main(int argc, char *argv[]) {

    qRegisterMetaType<size_t>("size_t");
    QApplication a(argc, argv);
     a.setWindowIcon(QIcon("/home/gemy/Desktop/qt/wirefish/src/images/p2.icon"));
    // Show splash screen
    SplashScreen splash;
    splash.exec();

    MainWindow w;
    w.show();

    return a.exec();
}
