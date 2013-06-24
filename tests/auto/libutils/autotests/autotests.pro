QT += testlib

include(../../../../buildspecs/config.pri)
include(../../../../libs/utils.pri)
include(../../../../libs/translationutils.pri)
include(../../../../libs/rpath.pri)

SOURCES = \
    ../../main.cpp

HEADERS += \
    ../../autotest.h

include(lib_utils/lib_utils.pri)
include(lib_translations/lib_translations.pri)

include(plugin_edrc/plugin_edrc.pri)
