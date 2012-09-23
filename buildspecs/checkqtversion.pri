#version check qt
contains(QT_VERSION, ^4\\.[0-6]\\..*) {
    message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
    error("Use at least Qt 4.7.")
}
include($${PWD}/catchspec.pri)

