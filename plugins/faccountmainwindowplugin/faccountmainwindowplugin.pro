TEMPLATE = lib
TARGET = MainWindow
PACKAGE_VERSION = 0.0.2

DEFINES += FACCOUNT_MAINWIN_LIBRARY

include(../fmf_plugins.pri)
include(faccountmainwindowplugin_dependencies.pri)


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
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/faccountmainwindowplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/faccountmainwindowplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/faccountmainwindowplugin_es.ts

OTHER_FILES = MainWindow.pluginspec

