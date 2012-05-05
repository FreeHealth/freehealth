TEMPLATE = subdirs

SUBDIRS = core \
    prevention \
    alert

core.subdir = coreplugin

prevention.subdir = preventionplugin
prevention.depends = core

alert.subdir = alertplugin
alert.depends = core
