TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    accountbase \
    account \
    printer \
    texteditor \
    listview \
    usermanager

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += printer
mainwindow.depends += account

accountbase.subdir   = ../../plugins/accountbaseplugin
accountbase.depends += core

account.subdir   = ../../plugins/accountplugin
account.depends += core
account.depends += texteditor
account.depends += printer
account.depends += accountbase
#account.depends += listview

printer.subdir   = ../../plugins/printerplugin
printer.depends += core
printer.depends += texteditor

listview.subdir   = ../../plugins/listviewplugin
listview.depends += core

texteditor.subdir   = ../../plugins/texteditorplugin
texteditor.depends += core

usermanager.subdir   = usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview

