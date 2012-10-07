include(interactiondb_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lInteractionDB_debug
    win32:LIBS   *= -lInteractionDB_d
} else {
    LIBS  *= -lInteractionDB
}
