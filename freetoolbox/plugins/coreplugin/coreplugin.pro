TEMPLATE = lib
TARGET = Core
PACKAGE_VERSION = 0.0.2

DEFINES += FREEICD
FREEICD = 1

BUILD_PATH_POSTFIXE = FreeIcd

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/coreplugin/common_core.pri)

QT *= sql network xml

HEADERS += coreimpl.h \
    appaboutpage.h \
    coreplugin.h \
    mainwindow.h \
    itoolpage.h \
    ftb_constants.h \
    globaltools.h \
    pathpreferences.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp \
    mainwindow.cpp \
    globaltools.cpp \
    pathpreferences.cpp

FORMS += mainwindow.ui \
    pathpreferences.ui

OTHER_FILES += Core.pluginspec
