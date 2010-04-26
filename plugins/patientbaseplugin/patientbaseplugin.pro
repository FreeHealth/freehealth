TEMPLATE = lib
TARGET = PatientBase
PACKAGE_VERSION = 0.0.2
DEFINES += PATIENTBASE_LIBRARY
include(../fmf_plugins.pri)
include(patientbaseplugin_dependencies.pri)
HEADERS = patientbaseplugin.h \
    patientbase_exporter.h \
    patientbase.h \
    patientdata.h \
    patientmodel.h \
    episodemodel.h \
    identitywidget.h \
    constants_db.h \
    constants_menus.h \
    constants_settings.h \
    constants_trans.h \
    patientselector.h \
    patientwidgetmanager.h \
    patientselector_p.h \
    patientsearchmode.h \
    patientbasepreferencespage.h
SOURCES = patientbaseplugin.cpp \
    patientbase.cpp \
    patientdata.cpp \
    patientmodel.cpp \
    episodemodel.cpp \
    identitywidget.cpp \
    patientselector.cpp \
    patientwidgetmanager.cpp \
    patientsearchmode.cpp \
    patientbasepreferencespage.cpp
FORMS += identitywidget.ui \
    patientselector.ui \
    patientbasepreferencespage.ui
OTHER_FILES = PatientBase.pluginspec
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/patientbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/patientbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/patientbaseplugin_es.ts
