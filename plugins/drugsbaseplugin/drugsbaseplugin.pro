TEMPLATE        = lib
TARGET          = DrugsBase
PACKAGE_VERSION = 0.0.2

DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(drugsbaseplugin_sources.pri)

OTHER_FILES += DrugsBase.pluginspec
