include(datapackplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDataPack_debug
    win32:LIBS   *= -lDataPack_d
} else {
    LIBS  *= -lDataPack
}
