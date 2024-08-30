QT       += core gui
QT += core concurrent


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    battery.cpp \
    device.cpp \
    eeg.cpp \
    main.cpp \
    mainwindow.cpp \
    pc.cpp \
    sessionmanager.cpp \
    signalprocessor.cpp \
    testui.cpp \
    wavegenerator.cpp \
    qcustomplot.cpp

HEADERS += \
    battery.h \
    defs.h \
    device.h \
    eeg.h \
    mainwindow.h \
    pc.h \
    sessionmanager.h \
    signalprocessor.h \
    testui.h \
    wavegenerator.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui \
    testUI.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
