TEMPLATE        = lib
TARGET          = ICD

DEFINES += ICD_LIBRARY

include(../../../plugins/icdplugin/icdplugin_sources.pri)

DEPENDPATH += ../

OTHER_FILES = ICD.pluginspec

