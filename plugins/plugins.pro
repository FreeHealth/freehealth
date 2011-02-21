TEMPLATE = subdirs

SUBDIRS = \
    listview \
    printer \
    core \
    patientbase \
    formmanager \
    texteditor \
    usermanager \
    mainwin \
    templates \
    saverestore \
    xmlio \
    basewidgets \
    aggir \
    drugs \
    drugsbase \
    icd \
    pmh \
    padtools \
    category \
#    alerts \

# root plugin
core.subdir = fmfcoreplugin

# simple plugins
listview.subdir   = listviewplugin
listview.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

padtools.subdir = padtoolsplugin
padtools.depends += core

saverestore.subdir   = saverestoreplugin
saverestore.depends += core

templates.subdir   = templatesplugin
templates.depends += core

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

# complex plugins
mainwin.subdir   = fmfmainwindowplugin
mainwin.depends += core
mainwin.depends += texteditor
mainwin.depends += printer
mainwin.depends += usermanager
mainwin.depends += formmanager

xmlio.subdir   = xmlioplugin
xmlio.depends += core
xmlio.depends += formmanager

basewidgets.subdir   = basewidgetsplugin
basewidgets.depends += core
basewidgets.depends += texteditor
basewidgets.depends += formmanager

aggir.subdir   = aggirplugin
aggir.depends += core
aggir.depends += formmanager

drugs.subdir   = drugsplugin
drugs.depends += core
drugs.depends += drugsbase
drugs.depends += texteditor
drugs.depends += templates
drugs.depends += formmanager

drugsbase.subdir   = drugsbaseplugin
drugsbase.depends += core
drugsbase.depends += templates
drugsbase.depends += printer

usermanager.subdir   = usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview

patientbase.subdir   = patientbaseplugin
patientbase.depends += core
patientbase.depends += usermanager

#alerts.subdir = alertsplugin
#alerts.depends += core

formmanager.subdir   = formmanagerplugin
formmanager.depends += core
formmanager.depends += usermanager

icd.subdir = icdplugin
icd.depends += core
icd.depends += formmanager

pmh.subdir = pmhplugin
pmh.depends += core
pmh.depends += icd
pmh.depends += listview
pmh.depends += texteditor
pmh.depends += category

category.subdir = categoryplugin
category.depends += core
category.depends += listview
