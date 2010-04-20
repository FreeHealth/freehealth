TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwin \
    printer \
    listview \
    texteditor \
    templates \
    saverestore \
    xmlio \
    basewidgets \
    aggir \
    drugs \
    drugsbase \
    usermanager \
    formmanager \
    patientbase \
#    identity \
#    alerts \

core.subdir = fmfcoreplugin

mainwin.subdir = fmfmainwindowplugin
mainwin.depends += core
mainwin.depends += texteditor
mainwin.depends += printer
mainwin.depends += usermanager
mainwin.depends += formmanager

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

xmlio.subdir   = xmlioplugin
xmlio.depends += core
xmlio.depends += formmanager

basewidgets.subdir   = basewidgetsplugin
basewidgets.depends += core
basewidgets.depends += formmanager

listview.subdir   = listviewplugin
listview.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core
texteditor.depends += formmanager

saverestore.subdir   = saverestoreplugin
saverestore.depends += core

aggir.subdir   = aggirplugin
aggir.depends += core
aggir.depends += formmanager

drugs.subdir   = drugsplugin
drugs.depends += core
drugs.depends += drugsbase
drugs.depends += texteditor
drugs.depends += templates
drugs.depends += formmanager

drugsbase.subdir = drugsbaseplugin
drugsbase.depends += core
drugsbase.depends += templates
drugsbase.depends += printer

usermanager.subdir   = usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview

patientbase.subdir = patientbaseplugin
patientbase.depends += core

alerts.subdir = alertsplugin
alerts.depends += core

templates.subdir = templatesplugin
templates.depends += core

formmanager.subdir = formmanagerplugin
formmanager.depends += core

#identity.subdir = identityplugin
#identity.depends += core
