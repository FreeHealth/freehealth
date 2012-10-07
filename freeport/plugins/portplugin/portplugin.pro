TEMPLATE        = lib
TARGET          = Port

DEFINES += PORT_LIBRARY
DEFINES += FREEPORT
FREEPORT = 1

BUILD_PATH_POSTFIXE = FreePort

INCLUDEPATH += ../
DEPENDPATH += ../

include( ../../../plugins/fmf_plugins.pri)
include( portplugin_dependencies.pri )

HEADERS += portplugin.h \
    port_exporter.h \
    constants.h \
    recoverabledata.h

SOURCES += portplugin.cpp \
    recoverabledata.cpp

OTHER_FILES += Port.pluginspec
