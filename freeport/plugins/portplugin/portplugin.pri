include(portplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lPort_debug
    win32:LIBS   *= -lPort_d
} else {
    LIBS  *= -lPort
}
