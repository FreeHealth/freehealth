# include path for toolkit
include( medicalutils_dependencies.pri )
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lMedicalUtils_debug
    win32:LIBS   *= -lMedicalUtils_d
} else {
    LIBS  *= -lMedicalUtils
}
