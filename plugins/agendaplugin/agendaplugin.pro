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
    availabilitycreatordialog.h \
    nextavailabiliystepviewer.h

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
    availabilitycreatordialog.cpp \
    nextavailabiliystepviewer.cpp

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

OTHER_FILES = Agenda.pluginspec

# include translations
TRANSLATION_NAME = agenda
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
