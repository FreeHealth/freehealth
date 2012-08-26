TEMPLATE = lib
TARGET = Core

DEFINES += FREEPAD
FREEPAD = 1
DEFINES += WITH_PAD

BUILD_PATH_POSTFIXE = FreePad

include(../../../plugins/coreplugin/common_core.pri)


INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += coreimpl.h \
    appaboutpage.h \
    coreplugin.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp

OTHER_FILES += Core.pluginspec
