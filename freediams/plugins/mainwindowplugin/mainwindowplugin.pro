TEMPLATE = lib
TARGET = MainWindow

DEFINES += FD_MAINWIN_LIBRARY
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

# include translations
TRANSLATION_NAME = fdmainwindow
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES = MainWindow.pluginspec

