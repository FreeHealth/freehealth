# toolkit library project file
TEMPLATE = lib
TARGET = toolkit
PACKAGE_VERSION = 0.1.0
!win32:VERSION = 0.1.0

# include config file
include( ../libsworkbench.pri )

# include sdk
INCLUDEPATH *= $${PWD}/sdk

# define Qt
QT *= sql \
    network \
    xml
CONFIG *= dll

# make library exportable
DEFINES *= TOOLKIT_CORE_BUILD

# Core
HEADERS += objects/tkExporter.h \
    objects/tkSendMessage.h \
    objects/tkExporter.h \
    objects/tkSettings.h \
    objects/tkLog.h \
    objects/tkGlobal.h \
    objects/tkTranslators.h \
    widgets/QButtonLineEdit.h \
    widgets/QPixLineEdit.h \
    widgets/tkDebugDialog.h \
    widgets/tkDebugDialog_p.h \
    database/tkDatabase.h \
    widgets/tkRichTextEditor.h \
    widgets/tkRichTextEditor_p.h \
    objects/tkSerializer.h \
    widgets/tkStringListView.h \
    objects/tkStringListModel.h \
    widgets/tkListView.h \
    widgets/tkListView_p.h \
    objects/tkTheme.h \
    objects/tkActionManager.h \
    widgets/tkLineEditEchoSwitcher.h \
    objects/tkTextDocumentExtra.h \
    objects/tkPrinter.h \
    objects/tkSerialNumber.h \
    widgets/tkScrollingWidget.h \
    widgets/tkAboutDialog.h \
    widgets/tkAboutDialog_p.h \
    objects/tkUpdateChecker_p.h \
    objects/tkUpdateChecker.h \
    widgets/tkPrinterPreviewer_p.h
SOURCES += objects/tkSettings.cpp \
    objects/tkLog.cpp \
    objects/tkGlobal.cpp \
    objects/tkTranslators.cpp \
    widgets/QButtonLineEdit.cpp \
    widgets/tkDebugDialog.cpp \
    database/tkDatabase.cpp \
    widgets/tkRichTextEditor.cpp \
    objects/tkSerializer.cpp \
    widgets/tkStringListView.cpp \
    objects/tkStringListModel.cpp \
    widgets/tkListView.cpp \
    objects/tkTheme.cpp \
    objects/tkActionManager.cpp \
    widgets/tkLineEditEchoSwitcher.cpp \
    objects/tkSerialNumber.cpp \
    widgets/tkScrollingWidget.cpp \
    widgets/tkAboutDialog.cpp \
    objects/tkUpdateChecker.cpp \
    widgets/tkPrinterPreviewer_p.cpp \
    objects/tkTextDocumentExtra.cpp
FORMS += widgets/tkDebugDialog.ui \
    widgets/tkTableDialog.ui \
    widgets/tkAboutDialog.ui \
    widgets/tkPrinterPreviewer_p.ui

# protected files
exists( $${PROTECTED_PATH} ):SOURCES += $${PROTECTED_PATH}/toolkit/objects/tkPrinter_Pro.cpp \
    $${PROTECTED_PATH}/toolkit/objects/tkSendMessage_Pro.cpp
else:SOURCES += objects/tkPrinter.cpp \
    objects/tkSendMessage.cpp

# translators
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/toolkit_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/toolkit_de.ts
