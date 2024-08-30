#include "mainwindow.h"
#include "defs.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<EEGWaveMap>("EEGWaveMap"); // Register the custom type
    qRegisterMetaType<EEGDomFreq>("EEGDomFreq");
    MainWindow w;
    w.show();
    return a.exec();
}
