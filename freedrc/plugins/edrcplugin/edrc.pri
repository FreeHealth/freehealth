include(edrc_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -leDRC_debug
    win32:LIBS   *= -leDRC_d
} else {
    LIBS  *= -leDRC
}

