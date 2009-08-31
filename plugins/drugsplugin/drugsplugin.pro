TEMPLATE        = lib
TARGET          = Drugs
PACKAGE_VERSION = 0.0.9

DEFINES += DRUGS_LIBRARY

include(../fmf_plugins.pri)
include( drugsplugin_dependencies.pri )

OTHER_FILES = Drugs.pluginspec


#include( ../formwidgets.pri )
#include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri )
#include( $${PACKAGE_LIBS_SOURCES}/sdk_usertoolkit.pri )
#include( $${PACKAGE_LIBS_SOURCES}/sdk_medicaltoolkit.pri )

DESTDIR = $$PACKAGE_PLUGINS_PATH

CONFIG *= sql plugins

QT *= sql network

#include( drugswidget.pri)

#HEADERS += drugswidget.h
#SOURCES += drugswidget.cpp

HEADERS += drugsplugin.h
SOURCES += drugsplugin.cpp
