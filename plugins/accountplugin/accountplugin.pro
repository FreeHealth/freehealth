TEMPLATE = lib
TARGET = Account

DEFINES += ACCOUNT_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeAccount

include(shared_sources.pri)

OTHER_FILES = Account.pluginspec
