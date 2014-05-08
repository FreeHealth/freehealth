# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

SUBDIRS = \
    core \
    icd10db \
    icd \
    zip \
    listview \
    biblio \
    datapack

core.subdir = coreplugin

icd10db.subdir = icd10db
icd10db.depends += core
icd10db.depends += icd

icd.subdir = icdplugin
icd.depends += core
icd.depends += listview
icd.depends += datapack

zip.subdir = zipcodes
zip.depends += core
zip.depends += datapack

listview.subdir = listviewplugin
listview.depends += core

biblio.subdir = biblio
biblio.depends += core

datapack.subdir = datapackplugin
datapack.depends += core
