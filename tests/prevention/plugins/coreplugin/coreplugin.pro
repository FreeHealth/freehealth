TEMPLATE = lib
TARGET = Core

DEFINES += PREVENTION
#PREVENTION = 1

BUILD_PATH_POSTFIXE = Prevention

include(../../../../plugins/coreplugin/common_core.pri)


INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += coreimpl.h \
    appaboutpage.h \
    coreplugin.h \
    commandlineparser.h \
    mainwindow.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp \
    commandlineparser.cpp \
    mainwindow.cpp

FORMS += mainwindow.ui

OTHER_FILES += Core.pluginspec
