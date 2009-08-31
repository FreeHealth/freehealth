# include path for toolkit
INCLUDEPATH *= $${PWD}/toolkit/sdk
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -ltoolkit_debug
    win32:LIBS   *= -ltoolkit_d
} else {
    LIBS  *= -ltoolkit
}
