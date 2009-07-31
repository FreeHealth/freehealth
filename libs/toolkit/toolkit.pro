# toolkit library project file
TEMPLATE = lib
TARGET = toolkit
PACKAGE_VERSION = 0.4.0
!win32:VERSION = $${PACKAGE_VERSION}

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

#include subprojects
include($${PWD}/richtexteditor/richtexteditor.pri)
include($${PWD}/actionmanager/actionmanager.pri)
include($${PWD}/contextmanager/contextmanager.pri)
include($${PWD}/listview/listview.pri)
include($${PWD}/printer/printer.pri)

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
    objects/tkSerializer.h \
    objects/tkTheme.h \
    widgets/tkLineEditEchoSwitcher.h \
    objects/tkSerialNumber.h \
    widgets/tkScrollingWidget.h \
    widgets/tkAboutDialog.h \
    widgets/tkAboutDialog_p.h \
    objects/tkUpdateChecker_p.h \
    objects/tkUpdateChecker.h \
    objects/tkConstantTranslations.h \
    widgets/tkHelpDialog.h \
    objects/tkUniqueIdentifier.h \
    objects/tkConstants.h
SOURCES += objects/tkSettings.cpp \
    objects/tkLog.cpp \
    objects/tkGlobal.cpp \
    objects/tkTranslators.cpp \
    widgets/QButtonLineEdit.cpp \
    widgets/tkDebugDialog.cpp \
    database/tkDatabase.cpp \
    objects/tkSerializer.cpp \
    objects/tkTheme.cpp \
    widgets/tkLineEditEchoSwitcher.cpp \
    objects/tkSerialNumber.cpp \
    widgets/tkScrollingWidget.cpp \
    widgets/tkAboutDialog.cpp \
    objects/tkUpdateChecker.cpp \
    objects/tkConstantTranslations.cpp \
    widgets/tkHelpDialog.cpp \
    objects/tkUniqueIdentifier.cpp
FORMS += widgets/tkDebugDialog.ui \
    widgets/tkAboutDialog.ui

# protected files
exists( $${PROTECTED_PATH} ):SOURCES += $${PROTECTED_PATH}/toolkit/objects/tkSendMessage_Pro.cpp \
    $${PROTECTED_PATH}/toolkit/objects/tkSerialNumber_pro.cpp
else:SOURCES += objects/tkSendMessage.cpp

# translators
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/toolkit_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/toolkit_de.ts
