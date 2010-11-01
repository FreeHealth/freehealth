TEMPLATE        = lib
TARGET          = AccountBase

DEFINES += FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

include(../../../plugins/accountbaseplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += AccountBase.pluginspec
