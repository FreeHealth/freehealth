#TEMPLATE = app

QT += gui testlib

include(../../../../libs/utils.pri)
include(../../../../buildspecs/config.pri)
include(../../../../libs/translationutils.pri)
include(../../../../libs/rpath.pri)

SOURCES += \
    tst_qbuttonlineedit.cpp

RESOURCES += \
    icons.qrc
