#-------------------------------------------------
#
# Project created by QtCreator 2018-03-28T14:08:54
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QLANToolkit
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


LIBS += D:\software\DevLib\npcap-sdk-0.1\Lib\Packet.lib
LIBS += D:\software\DevLib\npcap-sdk-0.1\Lib\wpcap.lib
LIBS += -lWs2_32
LIBS += -lIphlpapi

INCLUDEPATH += D:\software\DevLib\npcap-sdk-0.1\Include

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    LANAttacker/synfloodattacker.cpp \
    LANAttacker/lanpcap.cpp \
    LANAttacker/arpattacker.cpp

HEADERS += \
        mainwindow.h \
    LANAttacker/synfloodattacker.h \
    LANAttacker/lanpcap.h \
    LANAttacker/arpattacker.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    res.qrc
