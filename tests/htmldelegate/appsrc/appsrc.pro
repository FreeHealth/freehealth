TEMPLATE = app
TARGET = datapacks

QT += xml network core gui

include(../../../buildspecs/config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/rpath.pri)

HEADERS = mainwindow.h \
    fakehtmlmodel.h

SOURCES = main.cpp \
    mainwindow.cpp \
    fakehtmlmodel.cpp

FORMS = mainwindow.ui

