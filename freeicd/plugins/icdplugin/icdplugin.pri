include(icdplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lICD_debug
    win32:LIBS   *= -lICD_d
} else {
    LIBS  *= -lICD
}
