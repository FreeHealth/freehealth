TEMPLATE        = lib
TARGET          = Agenda

DEFINES += AGENDA_LIBRARY
DEFINES += FREEMEDFORMS

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( agendaplugin_dependencies.pri )

HEADERS = agendaplugin.h agenda_exporter.h \
    constants.h \
    agendabase.h \
    icalendarevent.h

SOURCES = agendaplugin.cpp \
    agendabase.cpp \
    icalendarevent.cpp

OTHER_FILES = Agenda.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_es.ts
