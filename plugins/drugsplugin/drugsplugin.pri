include(drugsplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDrugs_debug
    win32:LIBS   *= -lDrugs_d
} else {
    LIBS  *= -lDrugs
}
