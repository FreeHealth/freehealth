TEMPLATE = lib
TARGET = MainWindow

DEFINES *= DDI_MAINWIN_LIBRARY

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

# include translations
TRANSLATION_NAME = ddi_mainwindow
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES = MainWindow.pluginspec

