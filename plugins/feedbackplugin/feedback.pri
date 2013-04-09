include(feedback_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lFeedback_debug
    win32:LIBS   *= -lFeedback_d
} else {
    LIBS  *= -lFeedback
}

