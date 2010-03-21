# toolkit library project file
TEMPLATE        = lib
TARGET          = TranslationUtils
PACKAGE_VERSION = 0.4.0

DEFINES *= TRANSLATIONUTILS_LIBRARY

# include config file
include( ../libsworkbench.pri )

CONFIG *= dll

HEADERS += \
    translationutils_exporter.h \
    constanttranslations.h \
    multilingualclasstemplate.h

SOURCES += constanttranslations.cpp

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/translationutils_fr.ts \
                $${SOURCES_TRANSLATIONS}/translationutils_de.ts \
                $${SOURCES_TRANSLATIONS}/translationutils_es.ts
