!with-old-account {
  message(Account Old version plugin included but not in the CONFIG var of the qmake process)
} else {
  message(Building Old Accountancy plugin)
}


TEMPLATE = lib
TARGET = Account

DEFINES += ACCOUNT_LIBRARY

include(shared_sources.pri)

OTHER_FILES = Account.pluginspec

HEADERS += \
    accountmode.h

SOURCES += \
    accountmode.cpp
