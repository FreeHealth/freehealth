TEMPLATE = lib
TARGET = MainWindow

DEFINES += FREEICD_MAINWIN_LIBRARY
include( ../../../plugins/fmf_plugins.pri)
include( mainwindowplugin_dependencies.pri )

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += mainwindowplugin.h \
    mainwindow_exporter.h \
    mainwindow.h \
#    preferences/userpreferences.h \


SOURCES += mainwindowplugin.cpp \
    mainwindow.cpp \
#    preferences/userpreferences.cpp \


FORMS += mainwindow.ui \
#    preferences/useroptionspage.ui

# include translations
TRANSLATION_NAME = freeicdmainwindow
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES += MainWindow.pluginspec

