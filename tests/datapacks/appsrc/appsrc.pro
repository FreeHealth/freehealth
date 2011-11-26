TEMPLATE = app
TARGET = datapacks

QT += xml network core gui

include(../../../config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/datapackutils.pri)

HEADERS = mainwindow.h

SOURCES = main.cpp \
    mainwindow.cpp

FORMS = mainwindow.ui

