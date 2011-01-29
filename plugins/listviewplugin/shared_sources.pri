DEFINES += LISTVIEW_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/listviewplugin_dependencies.pri)

HEADERS += \
    $${PWD}/listview.h \
    $${PWD}/listview_p.h \
    $${PWD}/listviewplugin.h \
    $${PWD}/listview_exporter.h \
    $${PWD}/stringlistmodel.h \
    $${PWD}/stringlistview.h \
    $${PWD}/fancytreeview.h \
#    $${PWD}/constants.h \
#    $${PWD}/listviewbase.h \
#    $${PWD}/simplecategorymodel.h \
#    $${PWD}/simplecategorycreator.h

SOURCES += \
    $${PWD}/listview.cpp \
    $${PWD}/listviewplugin.cpp \
    $${PWD}/stringlistmodel.cpp \
    $${PWD}/stringlistview.cpp \
    $${PWD}/fancytreeview.cpp \
#    $${PWD}/listviewbase.cpp \
#    $${PWD}/simplecategorymodel.cpp \
#    $${PWD}/simplecategorycreator.cpp


FORMS += \
    $${PWD}/fancytreeview.ui \
 #   $${PWD}/simplecategorycreator.ui
