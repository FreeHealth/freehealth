include(listviewplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lListView_debug
    win32:LIBS   *= -lListView_d
} else {
    LIBS  *= -lListView
}
