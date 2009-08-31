
SOURCES += $${PWD}/coreplugin.cpp \
    $${PWD}/coremanager/mfCore.cpp \
    $${PWD}/crashmanager/mfCrashRecovererDialog.cpp \
    $${PWD}/crashmanager/mfRecovererThread.cpp \
    $${PWD}/dialogs/mfDialogInterpret.cpp \
    $${PWD}/iomanager/mfIOPlace.cpp \
    $${PWD}/maininterface/mfMainWidget.cpp \
    $${PWD}/maininterface/mfMainWindow.cpp \
    $${PWD}/objectmanager/mfObject.cpp \
    $${PWD}/objectmanager/mfObjectFundamental.cpp \
    $${PWD}/pluginsmanager/mfAbstractWidget.cpp \
    $${PWD}/printmanager/mfPrinter.cpp \
    $${PWD}/scriptmanager/mfScriptor.cpp \
    $${PWD}/pluginsmanager/mfPluginsManager.cpp \
    $${PWD}/mfGlobal.cpp \
    $${PWD}/settingsmanager/mfSettings.cpp \
    $${PWD}/aboutdialog/mfAboutDialog.cpp
HEADERS += $${PWD}/coreplugin.h \
    $${PWD}/coremanager/mfCore.h \
    $${PWD}/crashmanager/mfCrashRecovererDialog.h \
    $${PWD}/crashmanager/mfRecovererThread.h \
    $${PWD}/dialogs/mfDialogInterpret.h \
    $${PWD}/iomanager/mfIOPlace.h \
    $${PWD}/maininterface/mfMainWidget.h \
    $${PWD}/maininterface/mfMainWindow.h \
    $${PWD}/objectmanager/mfObjectFundamental.h \
    $${PWD}/objectmanager/mfObject.h \
    $${PWD}/pluginsmanager/mfBaseInterface.h \
    $${PWD}/pluginsmanager/mfAbstractWidget.h \
    $${PWD}/pluginsmanager/mfFormWidgetInterface.h \
    $${PWD}/pluginsmanager/mfIOInterface.h \
    $${PWD}/printmanager/mfPrinter.h \
    $${PWD}/scriptmanager/mfScriptor.h \
    $${PWD}/pluginsmanager/mfPluginsManager.h \
    $${PWD}/mfGlobal.h \
    $${PWD}/settingsmanager/mfSettings.h \
    $${PWD}/mfConstants.h \
    $${PWD}/aboutdialog/mfAboutDialog.h
FORMS += $${PWD}/ui/mfWidgetSelector.ui \
    $${PWD}/ui/mfMainWidgetUi.ui \
    $${PWD}/ui/mfCrashRecoverUi.ui \
    $${PWD}/dialogs/mfDialogInterpret.ui
#RESOURCES = application.qrc
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/fmf_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/fmf_de.ts
