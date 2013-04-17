TEMPLATE = app
TARGET = LibUtilsTester

QT += xml network core gui

include(../../../buildspecs/config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/rpath.pri)

SOURCES = main.cpp \
    widgets_unit_test.cpp \
    html_unit_tests.cpp

RESOURCES += \
    icons.qrc

HEADERS += \
    widgets_unit_test.h \
    html_unit_tests.h
