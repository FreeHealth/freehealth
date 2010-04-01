include(accountbaseplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAccountBase_debug
    win32:LIBS   *= -lAccountBased
} else {
    LIBS  *= -lAccountBase
}
