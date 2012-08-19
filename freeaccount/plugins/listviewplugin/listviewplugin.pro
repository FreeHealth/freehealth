TEMPLATE        = lib
TARGET          = ListView

with-pad:DEFINES+=WITH_PAD
DEFINES *= FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/listviewplugin/shared_sources.pri)

OTHER_FILES += ListView.pluginspec
