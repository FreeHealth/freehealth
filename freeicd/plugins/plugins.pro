TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    icd \
    printer \
    texteditor \
    user \
    views

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += icd
mainwindow.depends += printer

icd.subdir = icdplugin
icd.depends += core
icd.depends += views

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

texteditor.subdir   = texteditorplugin
texteditor.depends += core

user.subdir   = usermanagerplugin
user.depends += core
user.depends += views
user.depends += printer
user.depends += texteditor

views.subdir   = listviewplugin
views.depends += core
