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
    mainwindowpreferenceswidget.ui \
    headerwidget.ui

# include translations
TRANSLATION_NAME = famainwindow
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES = MainWindow.pluginspec

