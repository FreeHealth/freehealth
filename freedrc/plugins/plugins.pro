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
edrc.depends += listview
edrc.depends += texteditor

# optional plugins
include(../../buildspecs/optionalplugins.pri)

with-pad {
    SUBDIRS += padtools
    padtools.subdir = padtoolsplugin
    padtools.depends += core
}
