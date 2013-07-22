DEFINES += ZIPCODES_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/zipcodesplugin_dependencies.pri )

HEADERS = $${PWD}/zipcodesplugin.h \
    $${PWD}/zipcodes_exporter.h \
    $${PWD}/zipcodescompleters.h \
    $${PWD}/zipcodeswidget.h \
    $${PWD}/zipcore.h

SOURCES = $${PWD}/zipcodesplugin.cpp \
    $${PWD}/zipcodescompleters.cpp \
    $${PWD}/zipcodeswidget.cpp \
    $${PWD}/zipcore.cpp

# include translations
TRANSLATION_NAME = zipcodes
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
