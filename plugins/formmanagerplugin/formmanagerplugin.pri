include(formmanagerplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lFormManager_debug
    win32:LIBS   *= -lFormManager_d
} else {
    LIBS  *= -lFormManager
}
