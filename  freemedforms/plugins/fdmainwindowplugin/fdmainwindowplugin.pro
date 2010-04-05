TEMPLATE = lib
TARGET = MainWindow
PACKAGE_VERSION = 0.0.2

DEFINES += FD_MAINWIN_LIBRARY

include(../fmf_plugins.pri)
include( fdmainwindowplugin_dependencies.pri )


HEADERS = mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h \
    medintux.h \
    mainwindowpreferences.h

SOURCES = mainwindowplugin.cpp \
    mainwindow.cpp \
    mainwindowpreferences.cpp

FORMS = mainwindow.ui \
    mainwindowpreferenceswidget.ui

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/fdmainwindowplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/fdmainwindowplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/fdmainwindowplugin_es.ts

OTHER_FILES = MainWindow.pluginspec

exists( $${SOURCES_PROTECTED_PATH} ):SOURCES += $${SOURCES_PROTECTED_PATH}/freediams/medintux_pro.cpp
else:SOURCES += $${PWD}/medintux.cpp
