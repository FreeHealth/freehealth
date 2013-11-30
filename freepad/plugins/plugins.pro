# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    printer \
    texteditor \
    padtools

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += padtools
mainwindow.depends += printer

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

texteditor.subdir   = texteditorplugin
texteditor.depends += core

padtools.subdir   = padtoolsplugin
padtools.depends += core
padtools.depends += texteditor
