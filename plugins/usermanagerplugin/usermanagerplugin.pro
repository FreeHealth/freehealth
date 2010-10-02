TEMPLATE        = lib
TARGET          = UserManager
PACKAGE_VERSION = 0.0.2

DEFINES += USERS_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(shared_sources.pri)

OTHER_FILES = UserManager.pluginspec
