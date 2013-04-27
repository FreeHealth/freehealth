QT += testlib

include(../../../../buildspecs/config.pri)
include(../../../../libs/utils.pri)
include(../../../../libs/translationutils.pri)
include(../../../../libs/rpath.pri)

SOURCES = \
    ../../main.cpp \
    testhtmlcontent.cpp \
    tst_html.cpp \
    os_config.cpp \
    tst_fileaccess.cpp \
    tst_datetime.cpp \
    tst_xml.cpp

HEADERS += \
    testhtmlcontent.h \
    ../../autotest.h

FORMS += \
    testhtmlcontent.ui
