include(calendarplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lCalendar_debug
    win32:LIBS   *= -lCalendar_d
} else {
    LIBS  *= -lCalendar
}
