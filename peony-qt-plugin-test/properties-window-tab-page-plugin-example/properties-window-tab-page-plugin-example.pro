#-------------------------------------------------
#
# Project created by QtCreator 2019-10-24T13:53:42
#
#-------------------------------------------------

QT       += widgets

TARGET = properties-window-tab-page-plugin-example
TEMPLATE = lib

DEFINES += PROPERTIESWINDOWTABPAGEPLUGINEXAMPLE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../../plugin-iface/plugin-iface.pri)

SOURCES += \
        properties-window-tab-page-examplefactory.cpp

HEADERS += \
        properties-window-tab-page-examplefactory.h \
        properties-window-tab-page-plugin-example_global.h 

DESTDIR = ../../testdir

unix {
    target.path = /usr/lib/peony-qt-extension
    INSTALLS += target
}
