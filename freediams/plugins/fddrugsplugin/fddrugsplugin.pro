    TEMPLATE        = lib
TARGET          = Drugs
PACKAGE_VERSION = 0.0.9

DEFINES += DRUGS_LIBRARY
DEFINES += FREEDIAMS

include(../../../plugins/fmf_plugins.pri)
include(../../../plugins/drugsplugin/drugsplugin_dependencies.pri )

OTHER_FILES = Drugs.pluginspec

CONFIG *= sql plugins

QT *= sql network

include(../../../plugins/drugsplugin/drugswidget.pri)

#HEADERS += drugswidget.h
#SOURCES += drugswidget.cpp

HEADERS += ../../../plugins/drugsplugin/drugsplugin.h \
../../../plugins/drugsplugin/drugswidgetfactory.h

SOURCES += ../../../plugins/drugsplugin/drugsplugin.cpp \
../../../plugins/drugsplugin/drugswidgetfactory.cpp
