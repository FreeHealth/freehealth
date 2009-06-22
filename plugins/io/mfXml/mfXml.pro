TARGET = mf_xmlio

include( ../io.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri )

DESTDIR =  $$FMF_PLUGINS_DIR
QT *= xml

SOURCES +=   mfXmlIO.cpp
HEADERS +=   mfXmlIO.h

TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/xmlio_fr.ts \
                $${FMF_GLOBAL_RESOURCES}/translations/xmlio_de.ts

