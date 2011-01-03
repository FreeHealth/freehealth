include(padplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lPad_debug
    win32:LIBS   *= -lPaf_d
} else {
    LIBS  *= -lPad
}
