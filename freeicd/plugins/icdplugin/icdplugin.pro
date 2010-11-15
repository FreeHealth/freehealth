TEMPLATE        = lib
TARGET          = ICD
PACKAGE_VERSION = 0.0.2

DEFINES += ICD_LIBRARY
FREEICD = 1

BUILD_PATH_POSTFIXE = FreeICD

include(../../../plugins/fmf_plugins.pri)
include( icdplugin_dependencies.pri )

include($${SOURCES_PLUGINS_PATH}/icdplugin/icdplugin_sources.pri)

DEPENDPATH += ../

OTHER_FILES = ICD.pluginspec

