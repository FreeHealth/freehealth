TEMPLATE        = lib
TARGET          = Port

DEFINES += PORT_LIBRARY
DEFINES += FREEPORT
FREEPORT = 1

BUILD_PATH_POSTFIXE = FreePort

INCLUDEPATH += ../
DEPENDPATH += ../

#include(../../../plugins/portplugin/shared_sources.pri)

HEADERS += portplugin.h \

SOURCES += portplugin.cpp \

OTHER_FILES += Port.pluginspec
