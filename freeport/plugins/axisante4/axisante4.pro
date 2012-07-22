TARGET = Axisante4
TEMPLATE = lib

DEFINES += AXISANTE4_LIBRARY
BUILD_PATH_POSTFIXE = FreePort

include(../../../plugins/fmf_plugins.pri)
include(axisante4_dependencies.pri)

HEADERS += \
    axisante4plugin.h\
    axisante4_exporter.h\
    axisante4constants.h \
    patientbase.h
        
SOURCES += \
    axisante4plugin.cpp \
    patientbase.cpp

OTHER_FILES = Axisante4.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/axisante4_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/axisante4_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/axisante4_es.ts

