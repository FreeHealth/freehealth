include(texteditorplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lTextEditor_debug
    win32:LIBS   *= -lTextEditor_d
} else {
    LIBS  *= -lTextEditor
}
