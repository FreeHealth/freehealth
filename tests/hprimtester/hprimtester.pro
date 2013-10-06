# first definitions
TEMPLATE = app
TARGET = hprimtester

UTILS=../../libs/utils/
TRANS=../../libs/translationutils/

INCLUDEPATH=../../libs

QT += sql xml network

HEADERS += \
    $${UTILS}/log.h \
    $${UTILS}/global.h \
    $${UTILS}/licenseterms.h \
    $${TRANS}/constants.h \
    $${UTILS}/hprimparser.h

SOURCES += main.cpp \
    $${UTILS}/log.cpp \
    $${UTILS}/global.cpp \
    $${UTILS}/licenseterms.cpp \
    $${TRANS}/constanttranslations.cpp \
    $${UTILS}/hprimparser.cpp



