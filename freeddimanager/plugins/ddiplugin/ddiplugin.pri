include(ddiplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDDIManager_debug
    win32:LIBS   *= -lDDIManager_d
} else {
    LIBS  *= -lDDIManager
}
