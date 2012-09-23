TEMPLATE = lib
TARGET = MainWindow

DEFINES *= FACCOUNT_MAINWIN_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include(mainwindowplugin_dependencies.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS = mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h \
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

