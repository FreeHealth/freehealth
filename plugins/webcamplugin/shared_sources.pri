DEFINES += WEBCAM_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/webcam_dependencies.pri)

HEADERS += \
    $${PWD}/webcamplugin.h \
    $${PWD}/webcam_exporter.h \
    $${PWD}/webcamconstants.h \
    $${PWD}/webcamphotoprovider.h
        
SOURCES += \
    $${PWD}/webcamplugin.cpp \
    $${PWD}/webcamphotoprovider.cpp

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/webcam_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_es.ts

