include(icd10db_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lIcd10DB_debug
    win32:LIBS   *= -lIcd10DB_d
} else {
    LIBS  *= -lIcd10DB
}
