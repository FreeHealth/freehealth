TEMPLATE = lib
TARGET = Account

DEFINES += ACCOUNT_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(shared_sources.pri)

OTHER_FILES = Account.pluginspec

HEADERS += \
    accountmode.h

SOURCES += \
    accountmode.cpp
