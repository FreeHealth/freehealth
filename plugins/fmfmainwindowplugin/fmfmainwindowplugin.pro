TEMPLATE = lib
TARGET = MainWindow

DEFINES += FMF_MAINWIN_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( fmfmainwindowplugin_dependencies.pri )
HEADERS = mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h \
    mainwindowpreferences.h \
    virtualdatabasepreferences.h \
    #documentprinter.h

SOURCES = mainwindowplugin.cpp \
    mainwindow.cpp \
    mainwindowpreferences.cpp \
    virtualdatabasepreferences.cpp \
    #documentprinter.cpp

FORMS = virtualbasepage.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/fmfmainwindowplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/fmfmainwindowplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/fmfmainwindowplugin_es.ts

OTHER_FILES = MainWindow.pluginspec
