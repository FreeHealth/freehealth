TEMPLATE        = lib
TARGET          = TranslationUtils

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

DEFINES *= TRANSLATIONUTILS_LIBRARY

# include config file
include( ../libsworkbench.pri )

QT *= network

HEADERS += \
    translationutils_exporter.h \
    constanttranslations.h \
    constants.h \
    multilingualclasstemplate.h \
    googletranslator.h \
    trans_account.h \
    trans_agenda.h \
    trans_countries.h \
    trans_current.h \
    trans_database.h \
    trans_datetime.h \
    trans_drugs.h \
    trans_editor.h \
    trans_filepathxml.h \
    trans_menu.h \
    trans_msgerror.h \
    trans_patient.h \
    trans_spashandupdate.h \
    trans_team.h \
    trans_titles.h \
    trans_user.h \
    trans_units.h

SOURCES += constanttranslations.cpp \
    googletranslator.cpp

# include translations
TRANSLATION_NAME = translations
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
