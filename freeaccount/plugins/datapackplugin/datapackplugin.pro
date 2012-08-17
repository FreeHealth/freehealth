TEMPLATE        = lib
TARGET          = DataPackPlugin

with-pad:DEFINES+=WITH_PAD
DEFINES += FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/datapackplugin/shared_sources.pri)

OTHER_FILES += DataPackPlugin.pluginspec

