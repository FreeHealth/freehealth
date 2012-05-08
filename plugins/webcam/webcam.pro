TARGET = Webcam
TEMPLATE = lib

DEFINES += WEBCAM_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms
PROVIDER = FreeMedForms

include(../fmf_plugins.pri)
include(webcam_dependencies.pri)

HEADERS += \
    webcamplugin.h \
    webcam_exporter.h \
    webcamconstants.h \
    webcamphotoprovider.h
        
SOURCES += \
    webcamplugin.cpp \
    webcamphotoprovider.cpp

OTHER_FILES = Webcam.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/webcam_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_es.ts

