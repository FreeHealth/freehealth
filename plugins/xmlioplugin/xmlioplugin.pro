TEMPLATE        = lib
TARGET          = XmlIO
PACKAGE_VERSION = 0.0.9

DEFINES += XMLIO_LIBRARY

QT *= xml sql

include(../fmf_plugins.pri)
include( xmlioplugin_dependencies.pri )

HEADERS = xmlioplugin.h xmlio_exporter.h xmlformio.h xmlformioconstants.h

SOURCES = xmlioplugin.cpp xmlformio.cpp

OTHER_FILES = XmlIO.pluginspec
