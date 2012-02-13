TEMPLATE        = lib
TARGET          = DataPackPlugin

DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(datapackplugin_dependencies.pri)

include(shared_sources.pri)

OTHER_FILES = DataPackPlugin.pluginspec
