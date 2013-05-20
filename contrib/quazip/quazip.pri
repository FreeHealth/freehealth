# include path for QuaZip
include(quazip_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lfreemedforms-quazip-wrapper_debug
    win32:LIBS   *= -lfreemedforms-quazip-wrapper_d
} else {
    LIBS  *= -lfreemedforms-quazip-wrapper
}
