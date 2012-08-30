include(../fmf_plugins.pri)
include(accountplugin_dependencies.pri)

DEFINES *= ACCOUNT_LIBRARY

#include plugin structure
HEADERS = $${PWD}/accountplugin.h \
    $${PWD}/constants.h \
    $${PWD}/account_exporter.h \
    $${PWD}/accountwidgetmanager.h \
    $${PWD}/accountview.h \
    $${PWD}/accountcontextualwidget.h

SOURCES = $${PWD}/accountplugin.cpp \
    $${PWD}/accountwidgetmanager.cpp \
    $${PWD}/accountview.cpp \
    $${PWD}/accountcontextualwidget.cpp

FORMS = $${PWD}/accountview.ui

#include preferences
HEADERS += $${PWD}/preferences/accountpreferences.h \
    $${PWD}/preferences/bankdetailspage.h \
    $${PWD}/preferences/availablemovementpage.h \
    $${PWD}/preferences/medicalprocedurepage.h \
    $${PWD}/preferences/sitespage.h \
    $${PWD}/preferences/insurancepage.h \
    $${PWD}/preferences/virtualdatabasecreator.h \
    $${PWD}/preferences/percentagespage.h \
    $${PWD}/preferences/assetsratespage.h \
    $${PWD}/preferences/distancerulespage.h

SOURCES += $${PWD}/preferences/accountpreferences.cpp \
    $${PWD}/preferences/bankdetailspage.cpp \
    $${PWD}/preferences/availablemovementpage.cpp \
    $${PWD}/preferences/medicalprocedurepage.cpp \
    $${PWD}/preferences/sitespage.cpp \
    $${PWD}/preferences/insurancepage.cpp \
    $${PWD}/preferences/virtualdatabasecreator.cpp \
    $${PWD}/preferences/percentagespage.cpp \
    $${PWD}/preferences/assetsratespage.cpp \
    $${PWD}/preferences/distancerulespage.cpp

FORMS += $${PWD}/preferences/accountuseroptionspage.ui \
    $${PWD}/preferences/bankdetailspage.ui \
    $${PWD}/preferences/availablemovementpage.ui \
    $${PWD}/preferences/medicalprocedurepage.ui \
    $${PWD}/preferences/sitespage.ui \
    $${PWD}/preferences/insurancepage.ui \
    $${PWD}/preferences/virtualdatabasecreator.ui \
    $${PWD}/preferences/percentagespage.ui \
    $${PWD}/preferences/assetsratespage.ui \
    $${PWD}/preferences/distancerulespage.ui \
    $${PWD}/preferences/accountdatabasedefaultswidget.ui

# include receipts
HEADERS += $${PWD}/receipts/receiptsmanager.h \
    $${PWD}/receipts/receiptsio.h \
    $${PWD}/receipts/findreceiptsvalues.h \
    $${PWD}/receipts/xmlcategoriesparser.h \
    $${PWD}/receipts/choicedialog.h \
    $${PWD}/receipts/constants.h \
    $${PWD}/receipts/distance.h \
    $${PWD}/receipts/controlreceiptsio.h \
    $${PWD}/receipts/preferredreceipts.h \
    $${PWD}/receipts/receiptviewer.h \
    $${PWD}/receipts/freetext.h  \
    $${PWD}/receipts/tools.h

FORMS += $${PWD}/receipts/findvaluesgui.ui \
    $${PWD}/receipts/choicedialog.ui \
    $${PWD}/receipts/controlreceipts.ui \
    $${PWD}/receipts/preferredreceipts.ui \
    $${PWD}/receipts/receiptviewer.ui \
    $${PWD}/receipts/freetextdialog.ui

SOURCES += $${PWD}/receipts/receiptsmanager.cpp \
    $${PWD}/receipts/receiptsio.cpp \
    $${PWD}/receipts/findreceiptsvalues.cpp \
    $${PWD}/receipts/choicedialog.cpp \
    $${PWD}/receipts/xmlcategoriesparser.cpp \
    $${PWD}/receipts/distance.cpp \
    $${PWD}/receipts/controlreceiptsIO.cpp \
    $${PWD}/receipts/preferredreceipts.cpp \
    $${PWD}/receipts/receiptviewer.cpp \
    $${PWD}/receipts/freetext.cpp \
    $${PWD}/receipts/tools.cpp


# include assets
HEADERS += $${PWD}/assets/assetsio.h \
    $${PWD}/assets/assetsmanager.h \
    $${PWD}/assets/assetsviewer.h

FORMS += $${PWD}/assets/assetsviewer.ui \

SOURCES += $${PWD}/assets/assetsio.cpp \
    $${PWD}/assets/assetsmanager.cpp \
    $${PWD}/assets/assetsviewer.cpp

# include movements
HEADERS += $${PWD}/movements/movementsviewer.h \
    $${PWD}/movements/movementsio.h \
    $${PWD}/movements/movementsmanager.h

FORMS += $${PWD}/movements/movementsviewer.ui

SOURCES += $${PWD}/movements/movementsviewer.cpp \
    $${PWD}/movements/movementsio.cpp \
    $${PWD}/movements/movementsmanager.cpp

# include ledger
HEADERS += $${PWD}/ledger/ledgerio.h \
    $${PWD}/ledger/ledgermanager.h \
    $${PWD}/ledger/ledgerviewer.h \
    $${PWD}/ledger/ledgeredit.h \
    $${PWD}/ledger/mythread.h

FORMS += $${PWD}/ledger/ledgerviewer.ui \
    $${PWD}/ledger/ledgeredit.ui

SOURCES += $${PWD}/ledger/ledgerio.cpp \
    $${PWD}/ledger/ledgermanager.cpp \
    $${PWD}/ledger/ledgerviewer.cpp \
    $${PWD}/ledger/ledgeredit.cpp \
    $${PWD}/ledger/mythread.cpp


TRANSLATIONS += $${SOURCES_TRANSLATIONS}/accountplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/accountplugin_es.ts
