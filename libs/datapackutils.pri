INCLUDEPATH *= $${PWD}
include(datapackutils_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDataPackUtils_debug
    win32:LIBS   *= -lDataPackUtils_d
} else {
    LIBS  *= -lDataPackUtils
}
