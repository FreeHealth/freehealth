TEMPLATE        = lib
TARGET          = XmlIO

DEFINES += XMLIO_LIBRARY

QT *= xml sql

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(xmlioplugin_dependencies.pri)

# manage the alert dependency in the pluginspec with QMAKE_SUBSTITUTES??

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

with-alerts{
    DEFINES += WITH_ALERTS
    OTHER_FILES = with-alerts/XmlIO.pluginspec
}else{
    OTHER_FILES = XmlIO.pluginspec
}

# translators
TRANSLATIONS = $${SOURCES_TRANSLATIONS_PATH}/xmlioplugin_fr.ts \
               $${SOURCES_TRANSLATIONS_PATH}/xmlioplugin_de.ts \
               $${SOURCES_TRANSLATIONS_PATH}/xmlioplugin_es.ts
