TEMPLATE = lib
TARGET = Account

DEFINES += ACCOUNT_LIBRARY
DEFINES += FREEACCOUNT

BUILD_PATH_POSTFIXE = FreeAccount

include(../fmf_plugins.pri)
include( accountplugin_dependencies.pri )

# this patch must be removed with the future new plugins wrapper in FreeAccount
DEPENDPATH += $${SOURCES_ROOT_PATH}/freeaccount/plugins

#include plugin structure
HEADERS = accountplugin.h \
    constants.h \
    account_exporter.h \
    accountwidgetmanager.h \
    accountview.h \
    accountcontextualwidget.h

SOURCES = accountplugin.cpp \
    accountwidgetmanager.cpp \
    accountview.cpp \
    accountcontextualwidget.cpp

FORMS = accountview.ui

#include preferences
HEADERS += \
    preferences/accountpreferences.h \
    preferences/bankdetailspage.h \
    preferences/availablemovementpage.h \
    preferences/medicalprocedurepage.h \
    preferences/sitespage.h \
    preferences/insurancepage.h \
    preferences/virtualdatabasecreator.h \
    preferences/percentagespage.h \
    preferences/distancerulespage.h

SOURCES += \
    preferences/accountpreferences.cpp \
    preferences/bankdetailspage.cpp \
    preferences/availablemovementpage.cpp \
    preferences/medicalprocedurepage.cpp \
    preferences/sitespage.cpp \
    preferences/insurancepage.cpp \
    preferences/virtualdatabasecreator.cpp \
    preferences/percentagespage.cpp \
    preferences/distancerulespage.cpp

FORMS += preferences/accountuseroptionspage.ui \
    preferences/bankdetailspage.ui \
    preferences/availablemovementpage.ui \
    preferences/medicalprocedurepage.ui \
    preferences/sitespage.ui \
    preferences/insurancepage.ui \
    preferences/virtualdatabasecreator.ui \
    preferences/percentagespage.ui \
    preferences/distancerulespage.ui

# include receipts
HEADERS += \
    receipts/receiptsmanager.h \
    receipts/receiptsIO.h \
    receipts/findReceiptsValues.h \
    receipts/xmlcategoriesparser.h \
    receipts/choiceDialog.h \
    receipts/constants.h \
    receipts/distance.h \
    receipts/receiptviewer.h

FORMS += receipts/findValuesGUI.ui \
    receipts/ChoiceDialog.ui \
    receipts/receiptviewer.ui

SOURCES += \
    receipts/receiptsmanager.cpp \
    receipts/receiptsIO.cpp \
    receipts/findReceiptsValues.cpp \
    receipts/choiceDialog.cpp \
    receipts/xmlcategoriesparser.cpp \   # redondant with Utils::readXml in <utils/global.h>
    receipts/distance.cpp \
    receipts/receiptviewer.cpp

# include assets
HEADERS += assets/assetsIO.h \
    assets/assetsmanager.h \
    assets/assetsViewer.h

FORMS += assets/assetsviewer.ui

SOURCES += assets/assetsIO.cpp \
    assets/assetsmanager.cpp \
    assets/assetsViewer.cpp

# include movements
HEADERS += movements/movementsviewer.h \
    movements/movementsIO.h \
    movements/movementsmanager.h

FORMS += movements/movementsviewer.ui

SOURCES += movements/movementsviewer.cpp \
    movements/movementsIO.cpp \
    movements/movementsmanager.cpp

# include ledger
HEADERS += ledger/ledgerIO.h \
    ledger/ledgermanager.h \
    ledger/ledgerviewer.h \
    ledger/ledgeredit.h \
    ledger/mythread.h

FORMS += ledger/ledgerviewer.ui \
    ledger/ledgeredit.ui

SOURCES += ledger/ledgerIO.cpp \
    ledger/ledgermanager.cpp \
    ledger/ledgerviewer.cpp \
    ledger/ledgeredit.cpp \
    ledger/mythread.cpp


TRANSLATIONS += $${SOURCES_TRANSLATIONS}/accountplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_es.ts

OTHER_FILES = Account.pluginspec
