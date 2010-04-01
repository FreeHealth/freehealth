# include path for QuaZip
#include( quazip_dependencies.pri )
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lquazip_debug
    win32:LIBS   *= -lquazip_d
} else {
    LIBS  *= -lquazip
}
