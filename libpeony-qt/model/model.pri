INCLUDEPATH += $$PWD

#include(../file-operation/file-operation.pri)

HEADERS += \
    $$PWD/file-item.h \
    $$PWD/file-item-model.h \
    $$PWD/file-item-proxy-filter-sort-model.h \
    $$PWD/side-bar-abstract-item.h \
    $$PWD/side-bar-model.h \
    $$PWD/side-bar-favorite-item.h \
    $$PWD/side-bar-personal-item.h \
    $$PWD/side-bar-file-system-item.h \
    $$PWD/side-bar-proxy-filter-sort-model.h \
    $$PWD/path-bar-model.h \
    $$PWD/path-completer.h

SOURCES += \
    $$PWD/file-item.cpp \
    $$PWD/file-item-model.cpp \
    $$PWD/file-item-proxy-filter-sort-model.cpp \
    $$PWD/side-bar-abstract-item.cpp \
    $$PWD/side-bar-model.cpp \
    $$PWD/side-bar-favorite-item.cpp \
    $$PWD/side-bar-personal-item.cpp \
    $$PWD/side-bar-file-system-item.cpp \
    $$PWD/side-bar-proxy-filter-sort-model.cpp \
    $$PWD/path-bar-model.cpp \
    $$PWD/path-completer.cpp
