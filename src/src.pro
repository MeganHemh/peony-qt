#-------------------------------------------------
#
# Project created by QtCreator 2019-06-20T09:45:20
#
#-------------------------------------------------

QT       += core gui x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = peony-qt

TEMPLATE = app

include(../libpeony-qt/libpeony-qt.pri)

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0
LIBS +=-lgio-2.0 -lglib-2.0 -lX11
CONFIG += c++11 link_pkgconfig no_keywords

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    peony-application.cpp \
    peony-main.cpp \

HEADERS += \
    peony-application.h \

INCLUDEPATH    += ../plugin-iface

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
