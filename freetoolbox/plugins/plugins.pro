TEMPLATE = subdirs

SUBDIRS = \
    core \
    drugsdb \
    interactiondb \
    icd10db
#    druginfos \


core.subdir = coreplugin

drugsdb.subdir   = drugsdb
drugsdb.depends += core

interactiondb.subdir = interactiondb
interactiondb.depends += core

druginfos.subdir = druginfos
druginfos.depends += core

icd10db.subdir = icd10db
icd10db.depends += core
