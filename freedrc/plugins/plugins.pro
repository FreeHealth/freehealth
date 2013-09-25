TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    printer \
    texteditor \
    listview \
#    zipcodes \
#    datapack \
#    identity \
    edrc

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += printer
mainwindow.depends += edrc

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

listview.subdir   = listviewplugin
listview.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

#zipcodes.subdir = zipcodesplugin
#zipcodes.depends += core

#datapack.subdir = datapackplugin
#datapack.depends += core

#identity.subdir = identityplugin
#identity.depends += core
#identity.depends += zipcodes

edrc.subdir   = edrcplugin
edrc.depends += core

# optional plugins
include(../../buildspecs/optionalplugins.pri)

with-pad {
    SUBDIRS += padtools
    padtools.subdir = padtoolsplugin
    padtools.depends += core
}
