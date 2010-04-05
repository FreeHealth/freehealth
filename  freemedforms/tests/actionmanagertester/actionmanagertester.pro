TARGET = actionmanagertester
CONFIG += console
TEMPLATE = app

# include general configuration
include( ../../config.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri)
QT *= sql \
    network \
    xml \
    gui

# sources
SOURCES += main.cpp \
           TestWidget.cpp \
           MainWindow.cpp

HEADERS += MainWindow.h \
           TestWidget.h
