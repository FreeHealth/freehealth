include(axisante4_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAxisante4_debug
    win32:LIBS   *= -lAxisante4_d
} else {
    LIBS  *= -lAxisante4
}

