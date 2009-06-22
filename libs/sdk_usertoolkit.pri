# include path for usertoolkit
INCLUDEPATH *= $${PWD}/usertoolkit/sdk
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lusertoolkit_debug
    win32:LIBS   *= -lusertoolkit_d
} else {
    LIBS  *= -lusertoolkit
}
