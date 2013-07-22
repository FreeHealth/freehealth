TEMPLATE = app
TARGET = DataPackTester

QT += xml network core gui

include(../../../buildspecs/config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/datapackutils.pri)
include(../../../libs/rpath.pri)

HEADERS = mainwindow.h

SOURCES = main.cpp \
    mainwindow.cpp

FORMS = mainwindow.ui

