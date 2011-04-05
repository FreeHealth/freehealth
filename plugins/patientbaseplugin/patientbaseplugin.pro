TEMPLATE = lib
TARGET = PatientBase
PACKAGE_VERSION = 0.0.2
DEFINES += PATIENTBASE_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(patientbaseplugin_dependencies.pri)
HEADERS = patientbaseplugin.h \
    patientbase_exporter.h \
    patientbase.h \
    patientdata.h \
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
    patientcreatorwizard.h
SOURCES = patientbaseplugin.cpp \
    patientbase.cpp \
    patientdata.cpp \
    patientmodel.cpp \
    identitywidget.cpp \
    patientselector.cpp \
    patientwidgetmanager.cpp \
    patientsearchmode.cpp \
    patientbasepreferencespage.cpp \
    patientbar.cpp \
    patientcreatorwizard.cpp
FORMS += identitywidget.ui \
    patientselector.ui \
    patientbasepreferencespage.ui \
    patientbar.ui \
    identityviewer.ui
OTHER_FILES = PatientBase.pluginspec
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_es.ts
