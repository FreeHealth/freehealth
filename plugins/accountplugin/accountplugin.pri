include(accountplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAccount_debug
    win32:LIBS   *= -lAccount_d
} else {
    LIBS  *= -lAccount
}
