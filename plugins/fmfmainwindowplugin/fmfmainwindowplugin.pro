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

# include translations
TRANSLATION_NAME = fmfmainwindow
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES = MainWindow.pluginspec
