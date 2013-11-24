# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    texteditor \
    port \
    axisante4

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

port.subdir = portplugin
port.depends += core

