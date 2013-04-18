TEMPLATE = app
TARGET = LibUtilsTester

QT += xml network core gui testlib

include(../../../../buildspecs/config.pri)
include(../../../../libs/utils.pri)
include(../../../../libs/translationutils.pri)
include(../../../../libs/rpath.pri)

SOURCES = \
    widgets_unit_test.cpp \
    testhtmlcontent.cpp \
    tst_html.cpp

RESOURCES += \
    icons.qrc

HEADERS += \
    widgets_unit_test.h \
    testhtmlcontent.h

FORMS += \
    testhtmlcontent.ui
