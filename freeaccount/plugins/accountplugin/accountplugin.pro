TEMPLATE        = lib
TARGET          = Account

with-pad:DEFINES+=WITH_PAD
DEFINES *= FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

include(../../../plugins/accountplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += Account.pluginspec
