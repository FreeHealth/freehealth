TEMPLATE        = lib
TARGET          = UserManager

DEFINES += USERS_LIBRARY
DEFINES += FREEMEDFORMS
with-pad { DEFINES += WITH_PAD }

BUILD_PATH_POSTFIXE = FreeMedForms

include(shared_sources.pri)

OTHER_FILES = UserManager.pluginspec

