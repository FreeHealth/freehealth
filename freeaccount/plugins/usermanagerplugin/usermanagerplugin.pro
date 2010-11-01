TEMPLATE        = lib
TARGET          = UserManager

DEFINES += FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

include(../../../plugins/usermanagerplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += UserManager.pluginspec
