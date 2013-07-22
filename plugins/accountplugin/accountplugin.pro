TEMPLATE = lib
TARGET = Account

DEFINES += ACCOUNT_LIBRARY

include(shared_sources.pri)

OTHER_FILES = Account.pluginspec

HEADERS += \
    accountmode.h

SOURCES += \
    accountmode.cpp
