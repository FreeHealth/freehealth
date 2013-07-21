TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    printer \
    texteditor \
    drugs \
    drugsbase \
    templates \
    listview \
#    saverestore \
    datapack \
    ddi


core.subdir = coreplugin

drugsbase.subdir   = drugsbaseplugin
drugsbase.depends += core
drugsbase.depends += templates
drugsbase.depends += printer

drugs.subdir   = drugsplugin
drugs.depends += drugsbase
drugs.depends += core
drugs.depends += listview
drugs.depends += texteditor
drugs.depends += templates

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += drugs
mainwindow.depends += printer
mainwindow.depends += templates

listview.subdir   = listviewplugin
listview.depends += core

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

#saverestore.subdir = saverestoreplugin
#saverestore.depends = core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

templates.subdir = templatesplugin
templates.depends += core
#templates.depends += mainwindow

datapack.subdir   = datapackplugin
datapack.depends += core

ddi.subdir   = druginteractionsplugin
ddi.depends += drugsbase
ddi.depends += core

# optional plugins
include(../../buildspecs/optionalplugins.pri)

with-pad {
    SUBDIRS += padtools
    padtools.subdir = padtoolsplugin
    padtools.depends += core
}

with-alerts {
    SUBDIRS += alerts
    alerts.subdir = alertplugin
    alerts.depends += core
}

# manage non free content
#include(../../buildspecs/config.pri)
#!isEmpty(HAS_NONFREE):include($${NONFREE_SOURCES_PLUGINS_PATH}/plugins.pri)
#message($${HAS_NONFREE}  //  $${NONFREE_SOURCES_PLUGINS_PATH})
