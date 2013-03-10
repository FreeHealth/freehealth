#-------------------------------------------------
#
# Project created by QtCreator 2013-03-08T18:45:16
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = numbertostring
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../libs

SOURCES += main.cpp \
    ../../libs/utils/numbertostring.cpp \
    ../../libs/translationutils/constanttranslations.cpp

HEADERS += ../../libs/utils/numbertostring.h \
    ../../libs/translationutils/constants.h \
    ../../libs/translationutils/trans_numbers.h
