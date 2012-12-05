TEMPLATE = lib
TARGET = PatientBase

DEFINES += PATIENTBASE_LIBRARY

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
    pixmapbutton.h \
    identityviewerwidget.h \
    patientmodelwrapper.h \
    filephotoprovider.h \
    urlphotoprovider.h \
    urlphotodialog.h \
    patientsearchedit.h \
    patientcore.h \
    basicsqlpatientmodel.h

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
    pixmapbutton.cpp \
    identityviewerwidget.cpp \
    patientmodelwrapper.cpp \
    filephotoprovider.cpp \
    urlphotoprovider.cpp \
    urlphotodialog.cpp \
    patientsearchedit.cpp \
    patientcore.cpp \
    basicsqlpatientmodel.cpp

FORMS += identitywidget.ui \
    patientselector.ui \
    patientbasepreferencespage.ui \
    patientbar.ui \
    identityviewer.ui \
    urlphotodialog.ui

OTHER_FILES = PatientBase.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/patientbaseplugin_es.ts
