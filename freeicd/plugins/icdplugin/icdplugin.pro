TEMPLATE        = lib
TARGET          = ICD
PACKAGE_VERSION = 0.0.2

DEFINES += ICD_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include( icdplugin_dependencies.pri )

include($${SOURCES_PLUGINS_PATH}/icdplugin/icdplugin_sources.pri)

DEPENDPATH += ../

OTHER_FILES = ICD.pluginspec

