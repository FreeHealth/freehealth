TEMPLATE = lib
TARGET = Core

include(../../../../plugins/coreplugin/common_core.pri)
#include($${PWD}/../rapidcomptaportingplugin/rapidcomptaportingplugin.pro)



INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += coreimpl.h \
    appaboutpage.h \
    coreplugin.h \
    commandlineparser.h \
    mainwindow.h \
    fakescriptmanager.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp \
    commandlineparser.cpp \
    mainwindow.cpp \
    fakescriptmanager.cpp

FORMS += mainwindow.ui

OTHER_FILES += Core.pluginspec
