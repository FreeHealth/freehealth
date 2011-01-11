TEMPLATE = lib
TARGET = AccountBase
PACKAGE_VERSION = 0.0.2
DEFINES += ACCOUNTBASE_LIBRARY

BUILD_PATH_POSTFIXE = FreeAccount

include(../fmf_plugins.pri)
include( accountbaseplugin_dependencies.pri )

HEADERS = accountbaseplugin.h \
    accountbase_exporter.h \
    accountbase.h \
    constants.h \
    bankaccountmodel.h \
    availablemovementmodel.h \
    workingplacesmodel.h \
    insurancemodel.h \
    accountmodel.h \
    assetmodel.h \
    depositmodel.h \
    movementmodel.h \
    percentmodel.h \
    rulesmodel.h \
    distancerulesmodel.h \
    thesaurusmodel.h \
    actsmodel. h \
    medicalproceduremodel.h

SOURCES = accountbaseplugin.cpp \
    accountbase.cpp \
    bankaccountmodel.cpp \
    availablemovementmodel.cpp \
    workingplacesmodel.cpp \
    insurancemodel.cpp \
    accountmodel.cpp \
    assetmodel.cpp \
    depositmodel.cpp \
    movementmodel.cpp \
    percentmodel.cpp \
    rulesmodel.cpp \
    distancerulesmodel.cpp \
    thesaurusmodel.cpp \
    actsmodel.cpp \
    medicalproceduremodel.cpp

OTHER_FILES = AccountBase.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/accountbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/accountbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/accountbaseplugin_es.ts
