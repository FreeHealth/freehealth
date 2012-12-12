include(identity_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lIdentity_debug
    win32:LIBS   *= -lIdentity_d
} else {
    LIBS  *= -lIdentity
}

