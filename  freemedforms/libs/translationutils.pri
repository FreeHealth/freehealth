# include path for translationutils
#include( translationutils_dependencies.pri )
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lTranslationUtils_debug
    win32:LIBS   *= -lTranslationUtils_d
} else {
    LIBS  *= -lTranslationUtils
}
