TEMPLATE = subdirs

SUBDIRS = core \
    accountbase \
    port

core.subdir = coreplugin

accountbase.subdir = accountbaseplugin
accountbase.depends = core

port.subdir = rapidcomptaportingplugin
port.depends = core
port.depends += accountbase
