TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    printer \
    texteditor \
    drugs \
    drugsbase \
    templates


core.subdir = ../../plugins/fdcoreplugin

drugsbase.subdir   = fddrugsbaseplugin
drugsbase.depends += core
drugsbase.depends += templates
drugsbase.depends += printer

mainwindow.subdir   = ../../plugins/fdmainwindowplugin
mainwindow.depends += core
mainwindow.depends += drugs
mainwindow.depends += printer
mainwindow.depends += templates

printer.subdir   = ../../plugins/printerplugin
printer.depends += core
printer.depends += texteditor

#listview.subdir   = ../../plugins/listviewplugin
#listview.depends += core

texteditor.subdir   = ../../plugins/texteditorplugin
texteditor.depends += core

drugs.subdir   = fddrugsplugin
drugs.depends += drugsbase
drugs.depends += core
#drugs.depends += printer
drugs.depends += texteditor
drugs.depends += templates

templates.subdir = ../../plugins/templatesplugin
templates.depends += core
#templates.depends += mainwindow
