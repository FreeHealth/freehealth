TEMPLATE = subdirs

SUBDIRS = core \
    prevention

core.subdir = coreplugin

prevention.subdir = preventionplugin
prevention.depends = core

