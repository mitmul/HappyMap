#-------------------------------------------------
#
# Project created by QtCreator 2012-02-24T06:46:14
#
#-------------------------------------------------

QT       += core gui

TARGET = HappyMap
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    CapCambus.cpp \
    OSCReceiver.cpp \
    OSCSender.cpp \
    KinectControl.cpp

HEADERS  += MainWindow.h \
    CapCambus.h \
    OSCReceiver.h \
    OSCSender.h \
    KinectControl.h

FORMS    += MainWindow.ui
