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
    drugsbase \
    usermanager \
    patientbase \
    alerts

core.subdir = fmfcoreplugin

mainwin.subdir = fmfmainwindowplugin
mainwin.depends += core
mainwin.depends += texteditor
mainwin.depends += printer
mainwin.depends += usermanager

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
drugs.depends += drugsbase
drugs.depends += printer
drugs.depends += texteditor
drugs.depends += listview

drugsbase.subdir = drugsbaseplugin
drugsbase.depends += core

usermanager.subdir   = usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview

patientbase.subdir = patientbaseplugin
patientbase.depends += core

alerts.subdir = alertsplugin
alerts.depends += core
