!with-account {
  message(Account2 plugin included but not in the CONFIG var of the qmake process)
} else {
  message(Building New Accountancy plugin)
}

TEMPLATE = lib
TARGET = Account2

include(shared_sources.pri)

OTHER_FILES = Account2.pluginspec
