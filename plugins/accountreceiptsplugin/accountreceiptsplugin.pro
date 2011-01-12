TEMPLATE = lib
TARGET = AccountReceipts
PACKAGE_VERSION = 0.0.2

DEFINES += ACCOUNTRECEIPTS_LIBRARY

BUILD_PATH_POSTFIXE = FreeAccount

INCLUDEPATH += ../accountbaseplugin

include(../fmf_plugins.pri)

# this patch must be removed with the future new plugins wrapper in FreeAccount
DEPENDPATH += $${SOURCES_ROOT_PATH}/freeaccount/plugins

include(accountreceiptsplugin_dependencies.pri)

QT *= sql \
    network \
    xml

# Input
HEADERS += accountreceipts_exporter.h \
    accountreceiptsplugin.h \
    #receipts.h \       
    receiptsmanager.h \   
    receiptsIO.h \
    findReceiptsValues.h \
    xmlcategoriesparser.h \
    choiceDialog.h \
    constants.h \
    receiptviewer.h 


FORMS += findValuesGUI.ui \
    ChoiceDialog.ui \
    receiptviewer.ui
    #ReceiptsMainDialog.ui \
    #ReceiptsWidget.ui \

SOURCES += accountreceiptsplugin.cpp \
    #receipts.cpp \
    receiptsmanager.cpp \ 
    receiptsIO.cpp \
    findReceiptsValues.cpp \
    choiceDialog.cpp \
    xmlcategoriesparser.cpp \   # redondant with Utils::readXml in <utils/global.h>
    receiptviewer.cpp 


OTHER_FILES += AccountReceipts.pluginspec
