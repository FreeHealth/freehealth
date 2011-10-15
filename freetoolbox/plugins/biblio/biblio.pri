include(biblio_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lBiblio_debug
    win32:LIBS   *= -lBiblio_d
} else {
    LIBS  *= -lBiblio
}
