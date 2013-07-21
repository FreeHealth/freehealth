include(zipcodesplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lZipCodes_debug
    win32:LIBS   *= -lZipCodes_d
} else {
    LIBS  *= -lZipCodes
}
