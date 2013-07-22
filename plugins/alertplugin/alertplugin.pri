include(alertplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAlert_debug
    win32:LIBS   *= -lAlert_d
} else {
    LIBS  *= -lAlert
}
