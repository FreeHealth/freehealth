include(categoryplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lCategory_debug
    win32:LIBS   *= -lCategory_d
} else {
    LIBS  *= -lCategory
}
