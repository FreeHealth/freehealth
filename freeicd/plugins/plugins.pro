# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    icd \
    printer \
    texteditor \
    user \
    views \
    zipcodes \
    datapack

core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += icd
mainwindow.depends += printer

icd.subdir = icdplugin
icd.depends += core
icd.depends += views

printer.subdir   = printerplugin
printer.depends += core
printer.depends += texteditor

texteditor.subdir   = texteditorplugin
texteditor.depends += core

user.subdir   = usermanagerplugin
user.depends += core
user.depends += views
user.depends += printer
user.depends += texteditor
user.depends += zipcodes

views.subdir   = listviewplugin
views.depends += core

zipcodes.subdir = zipcodesplugin
zipcodes.depends += core

datapack.subdir   = datapackplugin
datapack.depends += core
