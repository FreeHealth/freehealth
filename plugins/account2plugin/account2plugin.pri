include(account2plugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAccount2_debug
    win32:LIBS   *= -lAccount2_d
} else {
    LIBS  *= -lAccount2
}
