TEMPLATE        = lib
TARGET          = AccountManager

DEFINES += FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

include(../../../plugins/accountmanagerplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += AccountManager.pluginspec
