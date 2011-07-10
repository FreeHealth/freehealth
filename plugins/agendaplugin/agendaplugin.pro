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
    agendawidgetmanager.h \
    useragendasviewer.h \
    agendamode.h \
    calendaritemmodel.h \
    calendaritemeditorpatientmapper.h \
    appointement.h
#    eventeditorwidget.h \
#    icalendarevent.h \
#    iusercalendar.h \
#    agendaeventsmodel.h \

SOURCES = agendaplugin.cpp \
    agendabase.cpp \
    agendawidgetmanager.cpp \
    useragendasviewer.cpp \
    agendamode.cpp \
    calendaritemmodel.cpp \
    calendaritemeditorpatientmapper.cpp \
    appointement.cpp
#    eventeditorwidget.cpp \
#    icalendarevent.cpp \
#    iusercalendar.cpp \
#    agendaeventsmodel.cpp \

OTHER_FILES = Agenda.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_es.ts

FORMS += \
    useragendasviewer.ui \
#    eventeditorwidget.ui \
    calendaritemeditorpatientmapper.ui
