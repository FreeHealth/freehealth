TEMPLATE = subdirs

SUBDIRS = \
    # Depend to core only
    core \
    listview \
    texteditor \
    templates \
    formmanager \
    zipcodes \
    datapack \
#    saverestore \    # unstable
    # Depend to more than one plugin
    printer \
    category \
    patientbase \
    usermanager \
    xmlio \
    aggir \
    basewidgets \
    drugsbase \
    drugs \
    druginteractions \
    icd \
    pmh \
    accountbase \
    account \
    agenda \
    script \
    mainwin


# root plugin
core.subdir = ../../plugins/fmfcoreplugin

# simple plugins
listview.subdir   = ../../plugins/listviewplugin
listview.depends += core

texteditor.subdir   = ../../plugins/texteditorplugin
texteditor.depends += core

saverestore.subdir   = ../../plugins/saverestoreplugin
saverestore.depends += core

templates.subdir   = ../../plugins/templatesplugin
templates.depends += core

formmanager.subdir   = ../../plugins/formmanagerplugin
formmanager.depends += core
formmanager.depends += listview

printer.subdir   = ../../plugins/printerplugin
printer.depends += core
printer.depends += texteditor

# complex plugins
mainwin.subdir   = ../../plugins/fmfmainwindowplugin
mainwin.depends += core
mainwin.depends += texteditor
mainwin.depends += printer
mainwin.depends += usermanager
mainwin.depends += formmanager
mainwin.depends += patientbase

xmlio.subdir   = ../../plugins/xmlioplugin
xmlio.depends += core
xmlio.depends += formmanager
xmlio.depends += category
xmlio.depends += pmh

basewidgets.subdir   = ../../plugins/basewidgetsplugin
basewidgets.depends += core
basewidgets.depends += texteditor
basewidgets.depends += formmanager
basewidgets.depends += patientbase

aggir.subdir   = ../../plugins/aggirplugin
aggir.depends += core
aggir.depends += formmanager

drugsbase.subdir   = ../../plugins/drugsbaseplugin
drugsbase.depends += core
drugsbase.depends += templates
drugsbase.depends += printer

druginteractions.subdir = ../../plugins/druginteractionsplugin
druginteractions.depends += core
druginteractions.depends += drugsbase

drugs.subdir   = ../../plugins/drugsplugin
drugs.depends += core
drugs.depends += drugsbase
drugs.depends += texteditor
drugs.depends += templates
drugs.depends += formmanager

usermanager.subdir   = ../../plugins/usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview
usermanager.depends += zipcodes
#usermanager.depends += agenda

agenda.subdir = ../../plugins/agendaplugin
agenda.depends += core
agenda.depends += patientbase
agenda.depends += usermanager
agenda.depends += listview

#alerts.subdir = alertsplugin
#alerts.depends += core

icd.subdir = ../../plugins/icdplugin
icd.depends += core
icd.depends += formmanager
icd.depends += listview

pmh.subdir = ../../plugins/pmhplugin
pmh.depends += core
pmh.depends += icd
pmh.depends += listview
pmh.depends += texteditor
pmh.depends += category
pmh.depends += formmanager

category.subdir = ../../plugins/categoryplugin
category.depends += core
category.depends += listview

accountbase.subdir = ../../plugins/accountbaseplugin
accountbase.depends += core

account.subdir   = ../../plugins/accountplugin
account.depends += core
account.depends += texteditor
account.depends += printer
account.depends += accountbase
account.depends += zipcodes
account.depends += listview

zipcodes.subdir = ../../plugins/zipcodesplugin
zipcodes.depends += core

script.subdir = ../../plugins/scriptplugin
script.depends += core
script.depends += formmanager

datapack.subdir = ../../plugins/datapackplugin
datapack.depends += core

with-webcam {
    SUBDIRS += webcam
    webcam.subdir = ../../plugins/webcamplugin
    webcam.depends += core
}

with-alerts {
    SUBDIRS += alerts
    alerts.subdir = ../../plugins/alertplugin
    alerts.depends += core
}

with-pad {
    SUBDIRS += padtools
    padtools.subdir = ../../plugins/padtoolsplugin
    padtools.depends += core
    padtools.depends += texteditor
}

patientbase.subdir   = ../../plugins/patientbaseplugin
patientbase.depends += core
patientbase.depends += listview
patientbase.depends += zipcodes
with-alerts { patientbase.depends += alerts }

# manage non free content
#include(../buildspecs/config.pri)
#!isEmpty(HAS_NONFREE):include($${NONFREE_SOURCES_PLUGINS_PATH}/plugins.pri)
#message($${HAS_NONFREE}  //  $${NONFREE_SOURCES_PLUGINS_PATH})
