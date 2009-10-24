TEMPLATE        = lib
TARGET          = MainWindow
PACKAGE_VERSION = 0.0.2

DEFINES += FMF_MAINWIN_LIBRARY

include(../fmf_plugins.pri)
include( fmfmainwindowplugin_dependencies.pri )

HEADERS = mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h \
    mainwindowpreferences.h

SOURCES = mainwindowplugin.cpp \
    mainwindow.cpp \
    mainwindowpreferences.cpp

FORMS = \
#    mainwindow.ui \
    mainwindowpreferenceswidget.ui

TRANSLATIONS += $${SOURCES_GLOBAL_RESOURCES}/translations/fmfmainwindowplugin_fr.ts \
    $${SOURCES_GLOBAL_RESOURCES}/translations/fmfmainwindowplugin_de.ts

OTHER_FILES = MainWindow.pluginspec
