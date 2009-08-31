TEMPLATE        = lib
TARGET          = EmptyPlugin
PACKAGE_VERSION = 0.0.2

DEFINES += EMPTY_LIBRARY

include(../fmf_plugins.pri)
include( emptyplugin_dependencies.pri )

HEADERS = emptyplugin.h empty_exporter.h

SOURCES = emptyplugin.cpp

OTHER_FILES = EmptyPlugin.pluginspec
