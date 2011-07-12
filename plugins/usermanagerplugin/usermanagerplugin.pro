TEMPLATE        = lib
TARGET          = UserManager

DEFINES += USERS_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(shared_sources.pri)

OTHER_FILES = UserManager.pluginspec

