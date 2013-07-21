# include path for toolkit
include( utils_dependencies.pri )
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lUtils_debug
    win32:LIBS   *= -lUtils_d
} else {
    LIBS  *= -lUtils
}
