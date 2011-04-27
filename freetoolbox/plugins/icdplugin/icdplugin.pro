TEMPLATE        = lib
TARGET          = ICD

DEFINES += ICD_LIBRARY
DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include( icdplugin_dependencies.pri )

include($${SOURCES_PLUGINS_PATH}/icdplugin/icdplugin_sources.pri)

DEPENDPATH += ../
INCLUDEPATH += ../

OTHER_FILES = ICD.pluginspec

