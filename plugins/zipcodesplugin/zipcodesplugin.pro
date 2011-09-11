TEMPLATE        = lib
TARGET          = ZipCodes

DEFINES += ZIPCODES_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( zipcodesplugin_dependencies.pri )

HEADERS = zipcodesplugin.h zipcodes_exporter.h \
    zipcodescompleters.h

SOURCES = zipcodesplugin.cpp \
    zipcodescompleters.cpp

OTHER_FILES = ZipCodes.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/zipcodesplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/zipcodesplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/zipcodesplugin_es.ts
