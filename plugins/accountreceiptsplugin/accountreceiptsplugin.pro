TEMPLATE = lib
TARGET = AccountReceipts
PACKAGE_VERSION = 0.0.2

DEFINES += ACCOUNTRECEIPTS_LIBRARY

BUILD_PATH_POSTFIXE = FreeAccount

include(../fmf_plugins.pri)

include(accountreceiptsplugin_dependencies.pri)

QT *= sql \
    network \
    xml

# Input
HEADERS += accountreceipts_exporter.h \
    accountreceiptsplugin.h \
    receipts.h \       
    receiptsmanager.h \   
    receiptsengine.h \
    findReceiptsValues.h \
    xmlcategoriesparser.h \
    receiptviewer.h

FORMS += ReceiptsMainDialog.ui \
    ReceiptsWidget.ui \
    findValuesGUI.ui \
    receiptviewer.ui

SOURCES += accountreceiptsplugin.cpp \
    receipts.cpp \
    receiptsmanager.cpp \ 
    receiptsengine.cpp \
    findReceiptsValues.cpp \
    xmlcategoriesparser.cpp \   # redondant with Utils::readXml in <utils/global.h>
    receiptviewer.cpp


OTHER_FILES += AccountReceipts.pluginspec
