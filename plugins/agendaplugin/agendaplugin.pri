include(agendaplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAgenda_debug
    win32:LIBS   *= -lAgenda_d
} else {
    LIBS  *= -lAgenda
}
