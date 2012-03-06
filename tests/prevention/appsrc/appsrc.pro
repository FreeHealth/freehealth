TEMPLATE = app
TARGET = prevention

QT += xml network core gui

include(../../../buildspecs/config.pri)
include(../../../libs/utils.pri)
include(../../../libs/translationutils.pri)
include(../../../libs/aggregation.pri)
include(../../../libs/extensionsystem.pri)
include(../../../libs/rpath.pri)

SOURCES = main.cpp \

