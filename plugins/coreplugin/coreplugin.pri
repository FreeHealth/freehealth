include(coreplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lCore_debug
    win32:LIBS   *= -lCore_d
} else {
    LIBS  *= -lCore
}
