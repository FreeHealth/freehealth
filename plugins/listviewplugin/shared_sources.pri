DEFINES += LISTVIEW_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/listviewplugin_dependencies.pri)

HEADERS += \
    $${PWD}/listview.h \
    $${PWD}/listview_p.h \
    $${PWD}/listviewplugin.h \
    $${PWD}/listview_exporter.h \
    $${PWD}/stringlistmodel.h \
    $${PWD}/stringlistview.h

SOURCES += \
    $${PWD}/listview.cpp \
    $${PWD}/listviewplugin.cpp \
    $${PWD}/stringlistmodel.cpp \
    $${PWD}/stringlistview.cpp
