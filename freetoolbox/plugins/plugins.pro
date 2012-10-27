TEMPLATE = subdirs

SUBDIRS = \
    core \
    drugsdb \
    icd10db \
    icd \
    zip \
    listview \
    biblio \
    datapack \
    account


core.subdir = coreplugin

drugsdb.subdir   = drugsdb
drugsdb.depends += core
drugsdb.depends += biblio
drugsdb.depends += listview
drugsdb.depends += icd10db

icd10db.subdir = icd10db
icd10db.depends += core
icd10db.depends += icd

icd.subdir = icdplugin
icd.depends += core
icd.depends += listview
icd.depends += datapack

zip.subdir = zipcodes
zip.depends += core

listview.subdir = listviewplugin
listview.depends += core

biblio.subdir = biblio
biblio.depends += core

datapack.subdir = datapackplugin
datapack.depends += core

account.subdir = accountplugin
account.depends += core
