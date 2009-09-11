TEMPLATE        = lib
TARGET          = MainWindow
PACKAGE_VERSION = 0.0.2

DEFINES += FD_MAINWIN_LIBRARY

include(../fmf_plugins.pri)
include( fdmainwindowplugin_dependencies.pri )

HEADERS = \
    mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h


SOURCES = \
    mainwindowplugin.cpp \
    mainwindow.cpp

FORMS = mainwindow.ui

OTHER_FILES = MainWindow.pluginspec

exists( $${SOURCES_PROTECTED_PATH} ):HEADERS += $${SOURCES_PROTECTED_PATH}/freediams/medintux_pro.h
else:HEADERS += $${PWD}/medintux.h
