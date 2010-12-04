include(drugsdb_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDrugsDB_debug
    win32:LIBS   *= -lDrugsDB_d
} else {
    LIBS  *= -lDrugsDB
}
