TEMPLATE = subdirs
SUBDIRS = src plugin libpeony-qt libpeony-qt/test \ #plugin-iface
    libpeony-qt/model/model-test \
    libpeony-qt/file-operation/file-operation-test \
    peony-qt-plugin-test

HEADERS +=

SOURCES += \
    libpeony-qt/controls/tool-bar/advance_search_bar.cpp
