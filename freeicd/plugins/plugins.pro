TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    icd \
    printer \
    texteditor

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += icd

icd.subdir = icdplugin
icd.depends += core

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

texteditor.subdir   = texteditorplugin
texteditor.depends += core
