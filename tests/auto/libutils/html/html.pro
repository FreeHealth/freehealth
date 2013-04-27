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
    tst_xml.cpp \
    tst_pass_log.cpp \
    tst_versionnumber.cpp \
    tst_printaxishelper.cpp

HEADERS += \
    testhtmlcontent.h \
    ../../autotest.h

FORMS += \
    testhtmlcontent.ui
