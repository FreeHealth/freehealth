include(patientbaseplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lPatientBase_debug
    win32:LIBS   *= -lPatientBase_d
} else {
    LIBS  *= -lPatientBase
}
