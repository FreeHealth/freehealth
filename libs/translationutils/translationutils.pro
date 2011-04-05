# toolkit library project file
TEMPLATE        = lib
TARGET          = TranslationUtils
PACKAGE_VERSION = 0.4.0

DEFINES *= TRANSLATIONUTILS_LIBRARY

# include config file
include( ../libsworkbench.pri )

QT *= network

HEADERS += \
    translationutils_exporter.h \
    constanttranslations.h \
    multilingualclasstemplate.h \
    googletranslator.h

SOURCES += constanttranslations.cpp \
    googletranslator.cpp

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/translationutils_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/translationutils_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/translationutils_es.ts
