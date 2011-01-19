include(padtoolsplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lPadTools_debug
    win32:LIBS   *= -lPadTools_d
} else {
    LIBS  *= -lPadTools
}
