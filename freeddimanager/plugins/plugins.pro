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
    ddi \
    drugsdb

core.subdir = coreplugin

ddi.subdir   = ddiplugin
ddi.depends += core
ddi.depends += texteditor
ddi.depends += listview

drugsdb.subdir   = drugsdbplugin
drugsdb.depends += core
drugsdb.depends += ddi

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += printer
mainwindow.depends += ddi

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

listview.subdir   = listviewplugin
listview.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core
