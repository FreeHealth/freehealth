DEFINES += WEBCAM_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/webcam_dependencies.pri)


TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/webcam_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_es.ts
