TEMPLATE = lib
TARGET = MainWindow

DEFINES += FREEPAD_MAINWIN_LIBRARY
FREEPAD = 1
DEFINES += WITH_PAD

BUILD_PATH_POSTFIXE = FreePad

include( ../../../plugins/fmf_plugins.pri)
include( mainwindowplugin_dependencies.pri )

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h \
    patientmodelwrapper.h \
    usermodelwrapper.h \
#    preferences/userpreferences.h \


SOURCES += mainwindowplugin.cpp \
    mainwindow.cpp \
    patientmodelwrapper.cpp \
    usermodelwrapper.cpp \
#    preferences/userpreferences.cpp \


FORMS += mainwindow.ui \
#    preferences/useroptionspage.ui

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/freepad_mainwindow_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/freepad_mainwindow_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/freepad_mainwindow_es.ts

OTHER_FILES += MainWindow.pluginspec

