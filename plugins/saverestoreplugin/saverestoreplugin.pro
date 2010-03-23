TEMPLATE        = lib
TARGET          = SaveRestore
PACKAGE_VERSION = 0.0.2

DEFINES += SAVERESTORE_LIBRARY

include(../fmf_plugins.pri)
include( saverestoreplugin_dependencies.pri )

HEADERS = saverestoreplugin.h saverestore_exporter.h saverestorepage.h

SOURCES = saverestoreplugin.cpp saverestorepage.cpp

FORMS = saverestorepage.ui

OTHER_FILES = SaveRestore.pluginspec

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/saverestoreplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/saverestoreplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/saverestoreplugin_es.ts

