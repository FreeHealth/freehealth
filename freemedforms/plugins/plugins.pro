TEMPLATE = subdirs

SUBDIRS = \
    # Depend to core only
    core \
    listview \
    texteditor \
    templates \
    zipcodes \
    datapack \
#    saverestore \    # unstable
    # Depend to more than one plugin
    formmanager \
    printer \
    identity \
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
    agenda \
    script \
    mainwin \
    tools

PLUGINS_PATH=../../plugins

# root plugin
core.subdir = $${PLUGINS_PATH}/fmfcoreplugin

# simple plugins
listview.subdir   = $${PLUGINS_PATH}/listviewplugin
listview.depends += core

texteditor.subdir   = $${PLUGINS_PATH}/texteditorplugin
texteditor.depends += core

saverestore.subdir   = $${PLUGINS_PATH}/saverestoreplugin
saverestore.depends += core

templates.subdir   = $${PLUGINS_PATH}/templatesplugin
templates.depends += core

# complex plugins
formmanager.subdir   = $${PLUGINS_PATH}/formmanagerplugin
formmanager.depends += core
formmanager.depends += listview

printer.subdir   = $${PLUGINS_PATH}/printerplugin
printer.depends += core
printer.depends += texteditor

identity.subdir = $${PLUGINS_PATH}/identityplugin
identity.depends += core
identity.depends += zipcodes

mainwin.subdir   = $${PLUGINS_PATH}/fmfmainwindowplugin
mainwin.depends += core
mainwin.depends += texteditor
mainwin.depends += printer
mainwin.depends += usermanager
mainwin.depends += formmanager
mainwin.depends += patientbase

xmlio.subdir   = $${PLUGINS_PATH}/xmlioplugin
xmlio.depends += core
xmlio.depends += formmanager
xmlio.depends += category
xmlio.depends += pmh

basewidgets.subdir   = $${PLUGINS_PATH}/basewidgetsplugin
basewidgets.depends += core
basewidgets.depends += texteditor
basewidgets.depends += formmanager
basewidgets.depends += patientbase
basewidgets.depends += identity

aggir.subdir   = $${PLUGINS_PATH}/aggirplugin
aggir.depends += core
aggir.depends += formmanager

drugsbase.subdir   = $${PLUGINS_PATH}/drugsbaseplugin
drugsbase.depends += core
drugsbase.depends += templates
drugsbase.depends += printer

druginteractions.subdir = $${PLUGINS_PATH}/druginteractionsplugin
druginteractions.depends += core
druginteractions.depends += drugsbase

drugs.subdir   = $${PLUGINS_PATH}/drugsplugin
drugs.depends += core
drugs.depends += drugsbase
drugs.depends += texteditor
drugs.depends += templates
drugs.depends += formmanager

usermanager.subdir   = $${PLUGINS_PATH}/usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview
usermanager.depends += zipcodes
usermanager.depends += identity

agenda.subdir = $${PLUGINS_PATH}/agendaplugin
agenda.depends += core
agenda.depends += patientbase
agenda.depends += usermanager
agenda.depends += listview

#alerts.subdir = alertsplugin
#alerts.depends += core

icd.subdir = $${PLUGINS_PATH}/icdplugin
icd.depends += core
icd.depends += formmanager
icd.depends += listview

pmh.subdir = $${PLUGINS_PATH}/pmhplugin
pmh.depends += core
pmh.depends += icd
pmh.depends += listview
pmh.depends += texteditor
pmh.depends += category
pmh.depends += formmanager

category.subdir = $${PLUGINS_PATH}/categoryplugin
category.depends += core
category.depends += listview

with-old-account {
    SUBDIRS += accountbase account
    accountbase.subdir = $${PLUGINS_PATH}/accountbaseplugin
    accountbase.depends += core
    accountbase.depends += datapack

    account.subdir   = $${PLUGINS_PATH}/accountplugin
    account.depends += core
    account.depends += texteditor
    account.depends += printer
    account.depends += accountbase
    account.depends += zipcodes
    account.depends += listview
}

with-account {
    SUBDIRS += account2
    account2.subdir   = $${PLUGINS_PATH}/account2plugin
    account2.depends += core
    account2.depends += datapack
    account2.depends += formmanager
    #account2.depends += texteditor
    #account2.depends += listview
}

zipcodes.subdir = $${PLUGINS_PATH}/zipcodesplugin
zipcodes.depends += core

script.subdir = $${PLUGINS_PATH}/scriptplugin
script.depends += core
script.depends += formmanager

datapack.subdir = $${PLUGINS_PATH}/datapackplugin
datapack.depends += core

with-webcam {
    SUBDIRS += webcam
    webcam.subdir = $${PLUGINS_PATH}/webcamplugin
    webcam.depends += core
}

with-alerts {
    SUBDIRS += alerts
    alerts.subdir = $${PLUGINS_PATH}/alertplugin
    alerts.depends += core
    #alerts.depends += patientbase

    xmlio.depends += alerts
}

with-pad {
    SUBDIRS += padtools
    padtools.subdir = $${PLUGINS_PATH}/padtoolsplugin
    padtools.depends += core
    padtools.depends += texteditor
}

patientbase.subdir   = $${PLUGINS_PATH}/patientbaseplugin
patientbase.depends += core
patientbase.depends += listview
patientbase.depends += zipcodes
patientbase.depends += formmanager
patientbase.depends += identity

tools.subdir   = $${PLUGINS_PATH}/toolsplugin
tools.depends += core


# manage non free content
#include(../buildspecs/config.pri)
#!isEmpty(HAS_NONFREE):include($${NONFREE_SOURCES_PLUGINS_PATH}/plugins.pri)
#message($${HAS_NONFREE}  //  $${NONFREE_SOURCES_PLUGINS_PATH})
