TEMPLATE        = lib
TARGET          = DrugsBase

DEFINES += FREEDIAMS
FREEDIAMS = 1

include(../../../plugins/drugsbaseplugin/drugsbaseplugin_sources.pri )

OTHER_FILES += DrugsBase.pluginspec
