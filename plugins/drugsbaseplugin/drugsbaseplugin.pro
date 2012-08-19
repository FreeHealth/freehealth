TEMPLATE        = lib
TARGET          = DrugsBase

DEFINES += FREEMEDFORMS
with-pad { DEFINES += WITH_PAD }

BUILD_PATH_POSTFIXE = FreeMedForms

include(drugsbaseplugin_sources.pri)

OTHER_FILES += DrugsBase.pluginspec
