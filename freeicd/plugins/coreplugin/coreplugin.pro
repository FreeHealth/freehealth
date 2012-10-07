TEMPLATE = lib
TARGET = Core

include(../../../plugins/coreplugin/common_core.pri)


INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += coreimpl.h \
    appaboutpage.h \
    coreplugin.h \
    commandlineparser.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp \
    commandlineparser.cpp

OTHER_FILES += Core.pluginspec
