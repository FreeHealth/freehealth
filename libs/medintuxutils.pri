# include path for translationutils
include( medintuxutils_dependencies.pri )
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lMedinTux_debug
    win32:LIBS   *= -lMedinTux_d
} else {
    LIBS  *= -lMedinTux
}
