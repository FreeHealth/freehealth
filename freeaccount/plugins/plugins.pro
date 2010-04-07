TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    accountbase \
    account \
    printer \
    texteditor \
    listview

core.subdir = ../../plugins/faccountcoreplugin

mainwindow.subdir   = ../../plugins/faccountmainwindowplugin
mainwindow.depends += core
mainwindow.depends += printer
mainwindow.depends += account

accountbase.subdir   = ../../plugins/accountbaseplugin
accountbase.depends += core

account.subdir   = ../../plugins/accountplugin
account.depends += core
account.depends += texteditor
account.depends += printer

printer.subdir   = ../../plugins/printerplugin
printer.depends += core
printer.depends += texteditor

listview.subdir   = ../../plugins/listviewplugin
listview.depends += core

texteditor.subdir   = ../../plugins/texteditorplugin
texteditor.depends += core

