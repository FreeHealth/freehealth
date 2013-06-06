TEMPLATE        = lib
TARGET          = Agenda

DEFINES += AGENDA_LIBRARY

include(../fmf_plugins.pri)
include( agendaplugin_dependencies.pri )

!with-agenda {
  error(Agenda plugin included but not in the CONFIG var of the qmake process)
} else {
  message(Building Agenda plugin)
}

HEADERS = agendaplugin.h agenda_exporter.h \
    constants.h \
    agendabase.h \
    agendawidgetmanager.h \
    usercalendarviewer.h \
    agendamode.h \
    calendaritemmodel.h \
    calendaritemeditorpatientmapper.h \
    appointment.h \
    usercalendar.h \
    usercalendareditor.h \
    usercalendarpageforuserviewer.h \
    usercalendarmodel.h \
    usercalendarwizardcreatorpage.h \
    usercalendarmodelfulleditorwidget.h \
    agendacore.h \
    agendapreferencespage.h \
    usercalendardelegatesmapper.h \
    nextavailabiliystepviewer.h \
    availabilityeditdialog.h \
    dayavailability.h

SOURCES = agendaplugin.cpp \
    agendabase.cpp \
    agendawidgetmanager.cpp \
    usercalendarviewer.cpp \
    agendamode.cpp \
    calendaritemmodel.cpp \
    calendaritemeditorpatientmapper.cpp \
    appointment.cpp \
    usercalendar.cpp \
    usercalendareditor.cpp \
    usercalendarpageforuserviewer.cpp \
    usercalendarmodel.cpp \
    usercalendarwizardcreatorpage.cpp \
    usercalendarmodelfulleditorwidget.cpp \
    agendacore.cpp \
    agendapreferencespage.cpp \
    usercalendardelegatesmapper.cpp \
    nextavailabiliystepviewer.cpp \
    availabilityeditdialog.cpp \
    dayavailability.cpp

FORMS += \
    calendaritemeditorpatientmapper.ui \
    usercalendarviewer.ui \
    usercalendareditor.ui \
    usercalendarwizardcreatorpage.ui \
    usercalendarmodelfulleditorwidget.ui \
    agendapreferencespage.ui \
    usercalendardelegatesmapper.ui \
    nextavailabiliystepviewer.ui \
    availabilityeditdialog.ui
#    agendauserviewerwidget.ui \

OTHER_FILES = Agenda.pluginspec

# include translations
TRANSLATION_NAME = agenda
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
