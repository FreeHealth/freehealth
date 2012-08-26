TEMPLATE        = lib
TARGET          = DataPackPlugin

DEFINES += FREEDIAMS
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/datapackplugin/shared_sources.pri)

OTHER_FILES += DataPackPlugin.pluginspec

