include(listviewplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDrugInteractions_debug
    win32:LIBS   *= -lDrugInteractions_d
} else {
    LIBS  *= -lDrugInteractions
}
