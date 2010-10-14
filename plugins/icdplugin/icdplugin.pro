TEMPLATE        = lib
TARGET          = ICD
PACKAGE_VERSION = 0.0.2

DEFINES += ICD_LIBRARY

include(../fmf_plugins.pri)
include( icdplugin_dependencies.pri )

HEADERS = icdplugin.h icd_exporter.h

SOURCES = icdplugin.cpp

OTHER_FILES = ICD.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/icdplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/icdplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/icdplugin_es.ts
