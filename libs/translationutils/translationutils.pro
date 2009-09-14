# toolkit library project file
TEMPLATE        = lib
TARGET          = TranslationUtils
PACKAGE_VERSION = 0.0.1

DEFINES *= TRANSLATIONUTILS_LIBRARY

# include config file
include( ../libsworkbench.pri )

CONFIG *= dll

# translators
TRANSLATIONS += $${SOURCES_GLOBAL_RESOURCES}/translations/translationutils_fr.ts \
    $${SOURCES_GLOBAL_RESOURCES}/translations/translationutils_de.ts

HEADERS += translationutils_exporter.h \
    constanttranslations.h \
    multilingualclasstemplate.h

SOURCES += constanttranslations.cpp
