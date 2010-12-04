TEMPLATE = lib
TARGET = Account
PACKAGE_VERSION = 0.0.2
DEFINES += ACCOUNT_LIBRARY

BUILD_PATH_POSTFIXE = FreeAccount

include(../fmf_plugins.pri)
include( accountplugin_dependencies.pri )

# this patch must be removed with the future new plugins wrapper in FreeAccount
DEPENDPATH += $${SOURCES_ROOT_PATH}/freeaccount/plugins


HEADERS = accountplugin.h \
    account_exporter.h \
    preferences/accountpreferences.h \
    accountwidgetmanager.h \
    preferences/bankdetailspage.h \
    preferences/availablemovementpage.h \
    preferences/medicalprocedurepage.h \
    accountview.h \
    preferences/virtualdatabasecreator.h

SOURCES = accountplugin.cpp \
    preferences/accountpreferences.cpp \
    accountwidgetmanager.cpp \
    preferences/bankdetailspage.cpp \
    preferences/availablemovementpage.cpp \
    preferences/medicalprocedurepage.cpp \
    accountview.cpp \
    preferences/virtualdatabasecreator.cpp

FORMS += preferences/accountuseroptionspage.ui \
    preferences/bankdetailspage.ui \
    preferences/availablemovementpage.ui \
    preferences/medicalprocedurepage.ui \
    accountview.ui \
    preferences/virtualdatabasecreator.ui


TRANSLATIONS += $${SOURCES_TRANSLATIONS}/accountplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_es.ts

OTHER_FILES = Account.pluginspec
