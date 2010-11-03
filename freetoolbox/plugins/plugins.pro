TEMPLATE = subdirs

SUBDIRS = \
    core \
    drugsdb \
    interactiondb


core.subdir = coreplugin

drugsdb.subdir   = drugsdb
drugsdb.depends += core

interactiondb.subdir = interactiondb
interactiondb.depends += core

icd10db.subdir = icd10db
icd10db.depends += core
