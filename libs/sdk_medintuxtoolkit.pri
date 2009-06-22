# include path for toolkit
INCLUDEPATH *= $${PWD}/medintuxtoolkit/sdk
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lmedintuxtoolkit_debug
    win32: LIBS  *= -lmedintuxtoolkit_d
} else {
    LIBS  *= -lmedintuxtoolkit
}
