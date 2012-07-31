TEMPLATE        = lib
TARGET          = PadTools

DEFINES += FREEDIAMS
DEFINES += WITH_PAD
FREEDIAMS = 1

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/padtoolsplugin/padtools_sources.pri)

OTHER_FILES += PadTools.pluginspec
