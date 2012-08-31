TEMPLATE = lib
TARGET = PatientBase

DEFINES += PATIENTBASE_LIBRARY
with-alerts{
    message(Build Alert part of PatientBase plugin)
    DEFINES += WITH_ALERTS
}
with-pad { DEFINES += WITH_PAD }

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(patientbaseplugin_dependencies.pri)

HEADERS = patientbaseplugin.h \
    patientbase_exporter.h \
    patientbase.h \
    patientmodel.h \
    identitywidget.h \
    constants_db.h \
    constants_menus.h \
    constants_settings.h \
    constants_trans.h \
    patientselector.h \
    patientwidgetmanager.h \
    patientsearchmode.h \
    patientbasepreferencespage.h \
    patientbar.h \
    patientcreatorwizard.h \
    patientlineeditcompletersearch.h \
    pixmapdelegate.h \
    pixmapbutton.h

SOURCES = patientbaseplugin.cpp \
    patientbase.cpp \
    patientmodel.cpp \
    identitywidget.cpp \
    patientselector.cpp \
    patientwidgetmanager.cpp \
    patientsearchmode.cpp \
    patientbasepreferencespage.cpp \
    patientbar.cpp \
    patientcreatorwizard.cpp \
    patientlineeditcompletersearch.cpp \
    pixmapdelegate.cpp \
    pixmapbutton.cpp

FORMS += identitywidget.ui \
    patientselector.ui \
    patientbasepreferencespage.ui \
    patientbar.ui \
    identityviewer.ui

with-alerts{
    HEADERS += with-alerts/patientbaralertplaceholder.h
    SOURCES += with-alerts/patientbaralertplaceholder.cpp
    OTHER_FILES = with-alerts/PatientBase.pluginspec
}else{
    OTHER_FILES = PatientBase.pluginspec
}

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_es.ts
