include(patientimport_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lpatientimport_debug
    win32:LIBS   *= -lpatientimport_d
} else {
    LIBS  *= -lpatientimport
}
