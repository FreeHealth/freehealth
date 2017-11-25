TEMPLATE        = lib
TARGET          = XmlIO

DEFINES += XMLIO_LIBRARY

QT *= xml \
      sql

BUILD_PATH_POSTFIXE = FreeHealth

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

equals(TEST, 1){
    SOURCES += \
        tests/tst_xmlformname.cpp
}

# include translations
TRANSLATION_NAME = xmlio
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

RESOURCES += \
    resource.qrc
