include($${PWD}/guardplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lGuard_debug
    win32:LIBS   *= -lGuard_d
} else {
    LIBS  *= -lGuard
}
