include(webcam_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lWebcam_debug
    win32:LIBS   *= -lWebcam_d
} else {
    LIBS  *= -lWebcam
}

