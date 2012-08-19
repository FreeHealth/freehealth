TEMPLATE = lib
TARGET = MainWindow

DEFINES += FD_MAINWIN_LIBRARY
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/fmf_plugins.pri)
include(mainwindowplugin_dependencies.pri)


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

