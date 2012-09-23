TEMPLATE        = lib
TARGET          = Agenda

DEFINES += AGENDA_LIBRARY

include(../fmf_plugins.pri)
include( agendaplugin_dependencies.pri )

HEADERS = agendaplugin.h agenda_exporter.h \
    constants.h \
    agendabase.h \
    agendawidgetmanager.h \
    usercalendarviewer.h \
    agendamode.h \
    calendaritemmodel.h \
    calendaritemeditorpatientmapper.h \
    appointement.h \
    usercalendar.h \
    usercalendareditor.h \
    usercalendarpageforuserviewer.h \
    usercalendarmodel.h \
    usercalendarwizardcreatorpage.h \
    usercalendarmodelfulleditorwidget.h \
    agendacore.h \
    agendapreferencespage.h \
    usercalendardelegatesmapper.h \
    availabilitycreatordialog.h \
    nextavailabiliystepviewer.h

SOURCES = agendaplugin.cpp \
    agendabase.cpp \
    agendawidgetmanager.cpp \
    usercalendarviewer.cpp \
    agendamode.cpp \
    calendaritemmodel.cpp \
    calendaritemeditorpatientmapper.cpp \
    appointement.cpp \
    usercalendar.cpp \
    usercalendareditor.cpp \
    usercalendarpageforuserviewer.cpp \
    usercalendarmodel.cpp \
    usercalendarwizardcreatorpage.cpp \
    usercalendarmodelfulleditorwidget.cpp \
    agendacore.cpp \
    agendapreferencespage.cpp \
    usercalendardelegatesmapper.cpp \
    availabilitycreatordialog.cpp \
    nextavailabiliystepviewer.cpp

OTHER_FILES = Agenda.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/agendaplugin_es.ts

FORMS += \
    calendaritemeditorpatientmapper.ui \
    usercalendarviewer.ui \
    usercalendareditor.ui \
    usercalendarwizardcreatorpage.ui \
    usercalendarmodelfulleditorwidget.ui \
    agendapreferencespage.ui \
    usercalendardelegatesmapper.ui \
    availabilitycreatordialog.ui \
    nextavailabiliystepviewer.ui
#    agendauserviewerwidget.ui \






