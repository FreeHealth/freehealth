TEMPLATE = lib
TARGET = Gir
PACKAGE_VERSION = 0.0.2
DEFINES += EMPTY_LIBRARY
include(../fmf_plugins.pri)
include( aggirplugin_dependencies.pri )
HEADERS = \
    aggirplugin.h \
    aggir_exporter.h \
    girwidget.h \
    girdatafactory.h

SOURCES = \
    aggirplugin.cpp \
    girwidget.cpp \
    girdatafactory.cpp

OTHER_FILES = Gir.pluginspec

FORMS += girwidget.ui
