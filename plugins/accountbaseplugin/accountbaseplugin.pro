TEMPLATE = lib
TARGET = AccountBase
PACKAGE_VERSION = 0.0.2
DEFINES += ACCOUNTBASE_LIBRARY

include(../fmf_plugins.pri)
include( accountbaseplugin_dependencies.pri )

HEADERS = accountbaseplugin.h \
    accountbase_exporter.h \
    accountbase.h \
    constants.h \
    bankaccountmodel.h \
    availablemovementmodel.h

SOURCES = accountbaseplugin.cpp \
    accountbase.cpp \
    bankaccountmodel.cpp \
    availablemovementmodel.cpp

OTHER_FILES = AccountBase.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/accountbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/accountbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/accountbaseplugin_es.ts
