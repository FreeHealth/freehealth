include(accountbaseplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAccountBase_debug
    win32:LIBS   *= -lAccountBase_d
} else {
    LIBS  *= -lAccountBase
}
