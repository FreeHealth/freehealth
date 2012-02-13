DEFINES += DATAPACK_PLUGIN_LIBRARY

include(../fmf_plugins.pri)
include(datapackplugin_dependencies.pri)

HEADERS = $${PWD}/datapackplugin.h $${PWD}/datapack_exporter.h \
    $${PWD}/constants.h \
    $${PWD}/datapackpreference.h

SOURCES = $${PWD}/datapackplugin.cpp \
    $${PWD}/datapackpreference.cpp

FORMS += \
    $${PWD}/datapackpreference.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/datapackplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/datapackplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/datapackplugin_es.ts




