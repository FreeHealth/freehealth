include(medicalhistoryplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lMedicalHistory_debug
    win32:LIBS   *= -lMedicalHistory_d
} else {
    LIBS  *= -lMedicalHistory
}
