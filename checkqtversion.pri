#version check qt
contains(QT_VERSION, ^4\\.[0-5]\\..*) {
    message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
    error("Use at least Qt 4.6.")
}
