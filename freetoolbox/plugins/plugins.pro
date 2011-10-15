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
    biblio


core.subdir = coreplugin

drugsdb.subdir   = drugsdb
drugsdb.depends += core

interactiondb.subdir = interactiondb
interactiondb.depends += core
interactiondb.depends += biblio

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

zip.subdir = zipcodes
zip.depends += core

listview.subdir = listviewplugin
listview.depends += core

biblio.subdir = biblio
biblio.depends += core
