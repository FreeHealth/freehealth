TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    icd


core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core

icd.subdir = icdplugin
icd.depends += core
