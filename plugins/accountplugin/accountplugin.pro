TEMPLATE = lib
TARGET = Account
PACKAGE_VERSION = 0.0.2
DEFINES += ACCOUNT_LIBRARY

BUILD_PATH_POSTFIXE = FreeAccount


include(../fmf_plugins.pri)
include( accountplugin_dependencies.pri )
HEADERS = accountplugin.h \
    account_exporter.h \
    preferences/accountpreferences.h \
    accountwidgetmanager.h \
    preferences/bankdetailspage.h \
    preferences/availablemovementpage.h \
    preferences/medicalprocedurepage.h \
    accountview.h

SOURCES = accountplugin.cpp \
    preferences/accountpreferences.cpp \
    accountwidgetmanager.cpp \
    preferences/bankdetailspage.cpp \
    preferences/availablemovementpage.cpp \
    preferences/medicalprocedurepage.cpp \
    accountview.cpp

FORMS += preferences/accountuseroptionspage.ui \
    preferences/bankdetailspage.ui \
    preferences/availablemovementpage.ui \
    preferences/medicalprocedurepage.ui \
    accountview.ui


TRANSLATIONS += $${SOURCES_TRANSLATIONS}/accountplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_es.ts

OTHER_FILES = Account.pluginspec
