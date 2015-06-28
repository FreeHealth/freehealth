include(translationutils.pri)
QT += sql xml network svg

# Qt5 QPrinter support
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
