QT += xml network core gui

TEMPLATE = app
TARGET = datapacks

include(../../../config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/datapackutils.pri)

HEADERS = mainwindow.h

SOURCES = main.cpp \
    mainwindow.cpp

FORMS = mainwindow.ui

