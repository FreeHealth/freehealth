include(templatesplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lTemplates_debug
    win32:LIBS   *= -lTemplates_d
} else {
    LIBS  *= -lTemplates
}
