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
    listview \
    guard

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
#mainwindow.depends += guard

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

listview.subdir   = listviewplugin
listview.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

guard.subdir = guardplugin
guard.depends += core
guard.depends += listview

# optional plugins
include(../../buildspecs/optionalplugins.pri)

with-pad {
    SUBDIRS += padtools
    padtools.subdir = padtoolsplugin
    padtools.depends += core
}
