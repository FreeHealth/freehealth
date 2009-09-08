TEMPLATE = subdirs

SUBDIRS = \
    core \
    printer \
    xmlio \
    basewidgets \
    listview \
    texteditor \
    aggir \
#    drugs

core.subdir = fmfcoreplugin

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


