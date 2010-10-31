include(mainwindowplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lMainWindow_debug
    win32:LIBS   *= -lMainWindow_d
} else {
    LIBS  *= -lMainWindow
}
