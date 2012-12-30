include(tools_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lTools_debug
    win32:LIBS   *= -lTools_d
} else {
    LIBS  *= -lTools
}

