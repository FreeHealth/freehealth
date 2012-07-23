include(%PluginName:l%_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -l%PluginName%_debug
    win32:LIBS   *= -l%PluginName%_d
} else {
    LIBS  *= -l%PluginName%
}
