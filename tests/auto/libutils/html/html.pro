TEMPLATE = app
TARGET = LibUtilsTester

QT += xml network core gui testlib

include(../../../../libs/utils.pri)
include(../../../../buildspecs/config.pri)
include(../../../../libs/translationutils.pri)
include(../../../../libs/rpath.pri)

SOURCES = \
    testhtmlcontent.cpp \
    tst_html.cpp

HEADERS += \
    testhtmlcontent.h

FORMS += \
    testhtmlcontent.ui
