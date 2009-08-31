# include path for toolkit
INCLUDEPATH *= $${PWD}/medicaltoolkit/sdk
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lmedicaltoolkit_debug
    win32:LIBS   *= -lmedicaltoolkit_d
} else {
    LIBS  *= -lmedicaltoolkit
}
