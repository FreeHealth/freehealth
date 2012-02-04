TEMPLATE = lib
TARGET = Core
PACKAGE_VERSION = 0.0.2

DEFINES += FREEICD
FREEICD = 1

BUILD_PATH_POSTFIXE = FreeICD

include(../../../plugins/coreplugin/common_core.pri)


INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += coreimpl.h \
    appaboutpage.h \
    coreplugin.h \
    commandlineparser.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp \
    commandlineparser.cpp

OTHER_FILES += Core.pluginspec
