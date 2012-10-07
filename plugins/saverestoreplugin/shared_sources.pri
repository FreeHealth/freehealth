DEFINES += SAVERESTORE_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/saverestoreplugin_dependencies.pri)

HEADERS = $${PWD}/saverestoreplugin.h $${PWD}/saverestore_exporter.h $${PWD}/saverestorepage.h

SOURCES = $${PWD}/saverestoreplugin.cpp $${PWD}/saverestorepage.cpp

FORMS = $${PWD}/saverestorepage.ui

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/saverestoreplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/saverestoreplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/saverestoreplugin_es.ts

