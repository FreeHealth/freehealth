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
    xmlformioconstants.h \
    xmlformcontentreader.h \
    xmliobase.h \
    constants.h \
    xmlformname.h

SOURCES = xmlioplugin.cpp \
    xmlformio.cpp \
    xmlformcontentreader.cpp \
    xmliobase.cpp \
    xmlformname.cpp

OTHER_FILES = XmlIO.pluginspec

# translators
TRANSLATIONS = $${SOURCES_TRANSLATIONS_PATH}/xmlioplugin_fr.ts \
               $${SOURCES_TRANSLATIONS_PATH}/xmlioplugin_de.ts \
               $${SOURCES_TRANSLATIONS_PATH}/xmlioplugin_es.ts
