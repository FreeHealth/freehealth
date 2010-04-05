include(usermanagerplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lUserManager_debug
    win32:LIBS   *= -lUserManager_d
} else {
    LIBS  *= -lUserManager
}
