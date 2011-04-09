TEMPLATE = lib
TARGET = Account

DEFINES += ACCOUNT_LIBRARY
DEFINES += FREEACCOUNT

BUILD_PATH_POSTFIXE = FreeAccount

include(shared_sources.pri)

OTHER_FILES = Account.pluginspec
