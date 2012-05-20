TEMPLATE = lib
TARGET = Core

BUILD_PATH_POSTFIXE = WebCam-Test

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
