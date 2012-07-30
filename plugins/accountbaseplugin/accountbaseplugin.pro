TEMPLATE = lib
TARGET = AccountBase

DEFINES += ACCOUNTBASE_LIBRARY
DEFINES += FREEMEDFORMS
with-pad { DEFINES += WITH_PAD }

BUILD_PATH_POSTFIXE = FreeMedForms

include(shared_sources.pri)

OTHER_FILES = AccountBase.pluginspec
