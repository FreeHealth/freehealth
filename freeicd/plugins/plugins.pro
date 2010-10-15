TEMPLATE = subdirs

SUBDIRS = \
    core \
    mainwindow \
    icd


core.subdir = coreplugin

mainwindow.subdir   = mainwindowplugin
mainwindow.depends += core
mainwindow.depends += icd

icd.subdir = icdplugin
icd.depends += core
