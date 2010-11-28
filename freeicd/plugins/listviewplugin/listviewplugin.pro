TEMPLATE        = lib
TARGET          = ListView

DEFINES += FREEICD
FREEICD = 1

BUILD_PATH_POSTFIXE = FreeICD

include(../../../plugins/listviewplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += ListView.pluginspec
