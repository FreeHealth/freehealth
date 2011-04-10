TEMPLATE = lib
TARGET = AccountBase

DEFINES += ACCOUNTBASE_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeAccount

include(shared_sources.pri)

OTHER_FILES = AccountBase.pluginspec
