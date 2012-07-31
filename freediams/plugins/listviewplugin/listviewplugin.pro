TEMPLATE        = lib
TARGET          = ListView

DEFINES += FREEDIAMS
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/listviewplugin/shared_sources.pri)

OTHER_FILES += ListView.pluginspec
