TEMPLATE        = lib
TARGET          = DrugInteractions

DEFINES += FREEDIAMS
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/druginteractionsplugin/shared_sources.pri )

OTHER_FILES += DrugInteractions.pluginspec

