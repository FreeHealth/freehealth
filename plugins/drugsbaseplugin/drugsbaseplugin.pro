TEMPLATE = lib
TARGET = DrugsBase
PACKAGE_VERSION = 0.0.2
DEFINES += DRUGSBASE_LIBRARY
include(../fmf_plugins.pri)
include(drugsbaseplugin_dependencies.pri)
HEADERS = drugsbaseplugin.h \
    drugsbase_exporter.h \
    drugsbase.h \
    interactionsbase.h \
    drugsdata.h \
    drugsinteraction.h \
    interactionsmanager.h \
    constants.h \
    drugsmodel.h \
    dosagemodel.h \
    drugsio.h \
    drugsbasemanager.h \
    globaldrugsmodel.h \
    dailyschememodel.h
SOURCES = drugsbaseplugin.cpp \
    drugsbase.cpp \
    interactionsbase.cpp \
    drugsdata.cpp \
    drugsinteraction.cpp \
    interactionsmanager.cpp \
    drugsmodel.cpp \
    dosagemodel.cpp \
    drugsio.cpp \
    drugsbasemanager.cpp \
    globaldrugsmodel.cpp \
    dailyschememodel.cpp
OTHER_FILES = DrugsBase.pluginspec
