TEMPLATE = subdirs

SUBDIRS = core \
    webcam

core.subdir = coreplugin

webcam.subdir = webcamplugin
webcam.depends = core

