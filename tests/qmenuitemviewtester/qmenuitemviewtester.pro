TEMPLATE = app
TARGET = qmenuitemviewtester
DESTDIR = $${PWD}

CONFIG *= qt warn_on thread x11 windows

QT *= core gui

qt:greaterThan(QT_MAJOR_VERSION, 4) {
    contains(QT, core): QT += concurrent
    contains(QT, gui): QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x040900
}

HEADERS += \
    ../../libs/utils/widgets/qmenuitemview.h

SOURCES += main.cpp \
    ../../libs/utils/widgets/qmenuitemview.cpp
