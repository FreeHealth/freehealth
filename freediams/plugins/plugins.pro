TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    printer \
    listview \
    texteditor \
    drugs \
    drugsbase

core.subdir = ../../plugins/fdcoreplugin

drugsbase.subdir   = ../../plugins/drugsbaseplugin
drugsbase.depends += core
drugsbase.depends += listview
drugsbase.depends += printer
drugsbase.depends += texteditor


mainwindow.subdir   = ../../plugins/fdmainwindowplugin
mainwindow.depends += core
mainwindow.depends += drugs
mainwindow.depends += printer

printer.subdir   = ../../plugins/printerplugin
printer.depends += core
printer.depends += texteditor

listview.subdir   = ../../plugins/listviewplugin
listview.depends += core

texteditor.subdir   = ../../plugins/texteditorplugin
texteditor.depends += core

drugs.subdir   = fddrugsplugin
drugs.depends += drugsbase
drugs.depends += core
drugs.depends += printer
drugs.depends += texteditor
drugs.depends += listview
