TEMPLATE        = lib
TARGET          = FormManager
PACKAGE_VERSION = 0.0.2

DEFINES += FORMMANAGER_LIBRARY

include(../fmf_plugins.pri)
include( formmanagerplugin_dependencies.pri )

HEADERS = formmanagerplugin.h formmanager_exporter.h \
    formmanager.h \
    iformio.h \
    iformitem.h \
    iformitemspec.h \
    iformitemscripts.h \
    iformitemvalues.h \
    iformwidgetfactory.h \
    iformitemdatafactory.h \
    formglobalmodel.h

SOURCES = formmanagerplugin.cpp \
    formmanager.cpp \
    iformitem.cpp \
    iformwidgetfactory.cpp \
    iformitemdatafactory.cpp \
    formglobalmodel.cpp

OTHER_FILES = FormManager.pluginspec
