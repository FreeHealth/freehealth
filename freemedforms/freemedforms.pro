# first definitions
TEMPLATE         = app
TARGET           = $$quote(FreeMedForms)
PACKAGE_VERSION  = 0.0.9

# include general configuration
include( ../config.pri )

# Prepare installation
FMF_BUILD = 1
INSTALL_DRUGS = 1
include( ../install.pri )

# include SDKs
include( $${PACKAGE_LIBS_SOURCES}/sdk_usertoolkit.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri )

# include Doc
!CONFIG(crosscompil) { 
    include( ../doc/manual-fmf.pri)
    PRE_TARGETDEPS += html_docs
}

# define CORE BUILD for fmf exporter
DEFINES *= FMF_CORE_BUILD

# Qt definitions
QT *= xml \
    network \
    script \
    sql

# precompil needed libs
mac:*-g++:LIBS *= -Wl,-all_load # import all symbols as the not used ones too
else:*-g++:LIBS *= -Wl,--whole-archive # import all symbols as the not used ones too
mac:*-g++:LIBS *= -dynamic
else:unix:*-g++:LIBS *= -rdynamic
PRE_TARGETDEPS *= $${PACKAGE_LIBS_SOURCES}/toolkit \
                  $${PACKAGE_LIBS_SOURCES}/usertoolkit
#                  $${PACKAGE_CONTRIBS}/quazip

# add compiled libs
CONFIG( debug, debug|release ) { 
    # Debug
#    unix:LIBS *= -L$${PACKAGE_CONTRIBS}/quazip \
#        -lquazip_debug
    
    # -lquazip_debug \
    # -ltoolkit_debug \
    # -lusertoolkit_debug
#    else:LIBS *= -L$${PACKAGE_CONTRIBS}/quazip \
#        -lquazip_d
    
    # -lquazip_d \
    # -lusertoolkit_d \
    # -ltoolkit_d
    win32-*g++:LIBS *= -Wl,--out-implib,$${PACKAGE_BUILD_PATH}/lib$${TARGET}.a
    win32-msvc*:LIBS *= /IMPLIB:$${PACKAGE_BUILD_PATH}/$${TARGET}.lib \
        -lshell32
}
else { 
    # Release
#    LIBS *= -L$${PACKAGE_CONTRIBS}/quazip \
#        -lquazip
    
    # -lquazip \
    # -lusertoolkit \
    # -ltoolkit
    win32-*g++:LIBS *= -Wl,--out-implib,$${PACKAGE_BUILD_PATH}/lib$${TARGET}.a,--enable-extra-pe-debug
    win32-msvc*:LIBS *= /IMPLIB:$${PACKAGE_BUILD_PATH}/$${TARGET}.lib \
        -lshell32
}
mac:*-g++:LIBS *= -Wl,-noall_load # stop importing all symbols
else:*-g++:LIBS *= -Wl,--no-whole-archive # stop importing all symbols

# Compile project
SOURCES += main.cpp \
    src/coremanager/mfCore.cpp \
    src/crashmanager/mfCrashRecovererDialog.cpp \
    src/crashmanager/mfRecovererThread.cpp \
    src/dialogs/mfDialogInterpret.cpp \
    src/iomanager/mfIOPlace.cpp \
    src/maininterface/mfMainWidget.cpp \
    src/maininterface/mfMainWindow.cpp \
    src/objectmanager/mfObject.cpp \
    src/objectmanager/mfObjectFundamental.cpp \
    src/pluginsmanager/mfAbstractWidget.cpp \
    src/printmanager/mfPrinter.cpp \
    src/scriptmanager/mfScriptor.cpp \
    src/pluginsmanager/mfPluginsManager.cpp \
    mfGlobal.cpp \
    src/settingsmanager/mfSettings.cpp \
    src/aboutdialog/mfAboutDialog.cpp
HEADERS += mfExporter.h \
    src/coremanager/mfCore.h \
    src/crashmanager/mfCrashRecovererDialog.h \
    src/crashmanager/mfRecovererThread.h \
    src/dialogs/mfDialogInterpret.h \
    src/iomanager/mfIOPlace.h \
    src/maininterface/mfMainWidget.h \
    src/maininterface/mfMainWindow.h \
    src/objectmanager/mfObjectFundamental.h \
    src/objectmanager/mfObject.h \
    src/pluginsmanager/mfBaseInterface.h \
    src/pluginsmanager/mfAbstractWidget.h \
    src/pluginsmanager/mfFormWidgetInterface.h \
    src/pluginsmanager/mfIOInterface.h \
    src/printmanager/mfPrinter.h \
    src/scriptmanager/mfScriptor.h \
    src/pluginsmanager/mfPluginsManager.h \
    mfGlobal.h \
    src/settingsmanager/mfSettings.h \
    mfConstants.h \
    src/aboutdialog/mfAboutDialog.h
FORMS += src/ui/mfWidgetSelector.ui \
    src/ui/mfMainWidgetUi.ui \
    src/ui/mfCrashRecoverUi.ui \
    src/dialogs/mfDialogInterpret.ui
RESOURCES = application.qrc
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/fmf_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/fmf_de.ts
