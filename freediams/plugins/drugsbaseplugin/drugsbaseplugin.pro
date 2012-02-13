TEMPLATE        = lib
TARGET          = DrugsBase

DEFINES += FREEDIAMS
FREEDIAMS = 1

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/drugsbaseplugin/drugsbaseplugin_sources.pri )

OTHER_FILES += DrugsBase.pluginspec

