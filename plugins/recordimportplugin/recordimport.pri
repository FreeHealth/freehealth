include(recordimport_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lrecordimport_debug
    win32:LIBS   *= -lrecordimport_d
} else {
    LIBS  *= -lrecordimport
}
