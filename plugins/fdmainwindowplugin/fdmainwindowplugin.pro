TEMPLATE = lib
TARGET = MainWindow
PACKAGE_VERSION = 0.0.2

DEFINES += FD_MAINWIN_LIBRARY
FREEDIAMS = 1

BUILD_PATH_POSTFIXE = FreeDiams

include(../fmf_plugins.pri)
include( fdmainwindowplugin_dependencies.pri )


HEADERS = mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h \
    medintux.h

SOURCES = mainwindowplugin.cpp \
    mainwindow.cpp \
    medintux.cpp

FORMS = mainwindow.ui

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/fdmainwindowplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/fdmainwindowplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/fdmainwindowplugin_es.ts

OTHER_FILES = MainWindow.pluginspec

