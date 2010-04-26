TEMPLATE = lib
TARGET = FormManager
PACKAGE_VERSION = 0.0.2
DEFINES += FORMMANAGER_LIBRARY
include(../fmf_plugins.pri)
include( formmanagerplugin_dependencies.pri )
HEADERS = formmanagerplugin.h \
    formmanager_exporter.h \
    formmanager.h \
    iformio.h \
    iformitem.h \
    iformitemspec.h \
    iformitemscripts.h \
    iformitemvalues.h \
    iformwidgetfactory.h \
    iformitemdatafactory.h \
    formglobalmodel.h \
    formmanagermode.h
SOURCES = formmanagerplugin.cpp \
    formmanager.cpp \
    iformitem.cpp \
    iformwidgetfactory.cpp \
    iformitemdatafactory.cpp \
    formglobalmodel.cpp \
    formmanagermode.cpp
OTHER_FILES = FormManager.pluginspec
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/formmanagerplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/formmanagerplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/formmanagerplugin_es.ts
