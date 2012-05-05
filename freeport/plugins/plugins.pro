TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    texteditor \
    port

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

port.subdir = portplugin
port.depends += core

