TEMPLATE = subdirs

SUBDIRS = \
    core \
    drugsdb \
    interactiondb \
    icd10db \
    druginfos \
    icd \
    zip \
    listview \
    biblio \
    datapack \
    account


core.subdir = coreplugin

drugsdb.subdir   = drugsdb
drugsdb.depends += core

interactiondb.subdir = interactiondb
interactiondb.depends += core
interactiondb.depends += biblio
interactiondb.depends += listview

druginfos.subdir = druginfos
druginfos.depends += core
druginfos.depends += interactiondb
druginfos.depends += icd10db

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
