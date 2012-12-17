TEMPLATE = app
TARGET = htmldelegate

QT += xml network core gui

include(../../../buildspecs/config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/rpath.pri)

HEADERS = mainwindow.h \
    ../../../libs/utils/widgets/htmldelegate.h \
    fakehtmlmodel.h

SOURCES = main.cpp \
    ../../../libs/utils/widgets/htmldelegate.cpp \
    mainwindow.cpp \
    fakehtmlmodel.cpp

FORMS = mainwindow.ui

