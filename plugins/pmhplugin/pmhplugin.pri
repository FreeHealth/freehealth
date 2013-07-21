include(pmhplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lPMH_debug
    win32:LIBS   *= -lPMH_d
} else {
    LIBS  *= -lPMH
}
