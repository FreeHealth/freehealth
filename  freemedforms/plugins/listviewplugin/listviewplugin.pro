TEMPLATE        = lib
TARGET          = ListView
PACKAGE_VERSION = 0.0.2

DEFINES += LISTVIEW_LIBRARY

include(../fmf_plugins.pri)
include( listviewplugin_dependencies.pri )

HEADERS = \
    listview.h \
    listview_p.h \
    listviewplugin.h \
    listview_exporter.h \
    stringlistmodel.h \
    stringlistview.h

SOURCES = \
    listview.cpp \
    listviewplugin.cpp \
    stringlistmodel.cpp \
    stringlistview.cpp

OTHER_FILES = ListView.pluginspec
