include(accountmanagerplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAccountManager_debug
    win32:LIBS   *= -lAccountManager_d
} else {
    LIBS  *= -lAccountManager
}
