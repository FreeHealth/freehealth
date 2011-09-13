TEMPLATE        = lib
TARGET          = ZipCodes

DEFINES *= FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

include(../../../plugins/zipcodesplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += ZipCodes.pluginspec
