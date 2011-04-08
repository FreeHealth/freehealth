TEMPLATE        = lib
TARGET          = XmlIO

DEFINES += XMLIO_LIBRARY

QT *= xml sql

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( xmlioplugin_dependencies.pri )

HEADERS = xmlioplugin.h \
    xmlio_exporter.h \
    xmlformio.h \
    xmlformioconstants.h

SOURCES = xmlioplugin.cpp \
    xmlformio.cpp

OTHER_FILES = XmlIO.pluginspec
