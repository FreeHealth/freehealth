QT       += core gui sql xml network

TARGET = qbletester

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../libs

SOURCES += main.cpp \
    ../../libs/utils/global.cpp \
    ../../libs/utils/log.cpp \
    ../../libs/utils/licenseterms.cpp \
    ../../libs/utils/widgets/qbuttonlineedit.cpp \
    ../../libs/translationutils/constanttranslations.cpp

HEADERS += \
    ../../libs/utils/global.h \
    ../../libs/utils/log.h \
    ../../libs/utils/licenseterms.h \
    ../../libs/utils/widgets/qbuttonlineedit.h \
    ../../libs/translationutils/constants.h \
    ../../libs/translationutils/trans_numbers.h

RESOURCES += \
    icons.qrc
