TEMPLATE        = lib
TARGET          = DrugInteractions

DEFINES += FREEMEDFORMS
with-pad { DEFINES += WITH_PAD }

BUILD_PATH_POSTFIXE = FreeMedForms

include($${PWD}/shared_sources.pri)

OTHER_FILES = DrugInteractions.pluginspec

