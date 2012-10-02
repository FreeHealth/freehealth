TEMPLATE = lib
TARGET = RapidPort

QT += sql

# Input
HEADERS += rapidcomptaportingplugin.h \
    rapidtofreeio.h rapidtofreeviewer.h connect.h

SOURCES += rapidcomptaportingplugin.cpp \
    rapidtofreeio.cpp rapidtofreeviewer.cpp connect.cpp

FORMS += rapidtofree.ui connexiondialog.ui

