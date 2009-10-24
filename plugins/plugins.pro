TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwin \
    printer \
    xmlio \
    basewidgets \
    listview \
    texteditor \
    aggir \
    drugs \
    usermanager

core.subdir = fmfcoreplugin

mainwin.subdir = fmfmainwindowplugin
mainwin.depends += core

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

xmlio.subdir   = xmlioplugin
xmlio.depends += core

basewidgets.subdir   = basewidgetsplugin
basewidgets.depends += core

listview.subdir   = listviewplugin
listview.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

aggir.subdir   = aggirplugin
aggir.depends += core

drugs.subdir   = drugsplugin
drugs.depends += core
drugs.depends += printer
drugs.depends += texteditor
drugs.depends += listview

usermanager.subdir   = usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview

