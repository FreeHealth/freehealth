TEMPLATE        = lib
TARGET          = ZipCodes

DEFINES += FREEICD
FREEICD = 1

BUILD_PATH_POSTFIXE = FreeICD

include(../../../plugins/zipcodesplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += ZipCodes.pluginspec
