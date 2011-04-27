TEMPLATE        = lib
TARGET          = ListView

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/listviewplugin/shared_sources.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

OTHER_FILES += ListView.pluginspec
