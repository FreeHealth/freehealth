include(printerplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lPrinter_debug
    win32:LIBS   *= -lPrinter_d
} else {
    LIBS  *= -lPrinter
}
