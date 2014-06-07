TEMPLATE = app
TARGET = DataPackTester

QT += xml network core gui

include(../../../buildspecs/config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/datapackutils.pri)
include(../../../libs/rpath.pri)

DEFINES	*= "TESTING_SOURCES_PATH=\"\\\"$${PWD}\\\"\""

HEADERS = mainwindow.h

SOURCES = main.cpp \
    mainwindow.cpp

FORMS = mainwindow.ui

