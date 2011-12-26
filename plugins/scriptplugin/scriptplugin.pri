include(scriptplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lScript_debug
    win32:LIBS   *= -lScript_d
} else {
    LIBS  *= -lScript
}
