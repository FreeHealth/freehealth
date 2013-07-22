TEMPLATE = subdirs

SUBDIRS = core \
    printer \
    texteditor \
    listview \
    zipcodes \
    identity \
    usermanager

core.subdir = coreplugin

# simple plugins
listview.subdir   = listviewplugin
listview.depends += core

texteditor.subdir   = texteditorplugin
texteditor.depends += core

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

identity.subdir = identityplugin
identity.depends += core
identity.depends += zipcodes

zipcodes.subdir = zipcodesplugin
zipcodes.depends += core

usermanager.subdir   = usermanagerplugin
usermanager.depends += core
usermanager.depends += printer
usermanager.depends += texteditor
usermanager.depends += listview
usermanager.depends += zipcodes
usermanager.depends += identity
