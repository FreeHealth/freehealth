DEFINES += LISTVIEW_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/listviewplugin_dependencies.pri)


HEADERS += \
    $${PWD}/listview.h \
    $${PWD}/listviewplugin.h \
    $${PWD}/listview_exporter.h \
    $${PWD}/stringlistmodel.h \
    $${PWD}/stringlistview.h \
    $${PWD}/fancytreeview.h \
    $${PWD}/extendedview.h \
    $${PWD}/tableview.h \
    $${PWD}/constants.h \
    $${PWD}/viewmanager.h \
    $${PWD}/treeview.h \
    $${PWD}/languagecombobox.h \
    $${PWD}/languagecomboboxdelegate.h \
    $${PWD}/countrycombobox.h


SOURCES += \
    $${PWD}/listview.cpp \
    $${PWD}/listviewplugin.cpp \
    $${PWD}/stringlistmodel.cpp \
    $${PWD}/stringlistview.cpp \
    $${PWD}/fancytreeview.cpp \
    $${PWD}/extendedview.cpp \
    $${PWD}/tableview.cpp \
    $${PWD}/viewmanager.cpp \
    $${PWD}/treeview.cpp \
    $${PWD}/languagecombobox.cpp \
    $${PWD}/languagecomboboxdelegate.cpp \
    $${PWD}/countrycombobox.cpp

FORMS += \
    $${PWD}/fancytreeview.ui \
