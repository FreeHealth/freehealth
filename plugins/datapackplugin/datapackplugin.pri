include(datapackplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDataPackPlugin_debug
    win32:LIBS   *= -lDataPackPlugin_d
} else {
    LIBS  *= -lDataPackPlugin
}
