TEMPLATE        = lib
TARGET          = Empty

DEFINES += EMPTY_LIBRARY

include(../fmf_plugins.pri)
include( emptyplugin_dependencies.pri )

HEADERS = emptyplugin.h empty_exporter.h

SOURCES = emptyplugin.cpp

OTHER_FILES = Empty.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/emptyplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/emptyplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/emptyplugin_es.ts
