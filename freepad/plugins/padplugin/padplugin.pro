TEMPLATE        = lib
TARGET          = Pad

DEFINES += PAD_LIBRARY
FREEPAD = 1

BUILD_PATH_POSTFIXE = FreePad

include( ../../../plugins/fmf_plugins.pri)
include( padplugin_dependencies.pri )

HEADERS = padplugin.h pad_exporter.h

SOURCES = padplugin.cpp

OTHER_FILES = Pad.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/padplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/padplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/padplugin_es.ts
