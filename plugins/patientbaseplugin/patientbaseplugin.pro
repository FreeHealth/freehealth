TEMPLATE = lib
TARGET = PatientBase

DEFINES += PATIENTBASE_LIBRARY

BUILD_PATH_POSTFIXE = FreeHealth

include(../fmf_plugins.pri)
include(patientbaseplugin_dependencies.pri)

HEADERS = patientbaseplugin.h \
    patientbase_exporter.h \
    patientbase.h \
    patientmodel.h \
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
    identityviewerwidget.h \
    patientmodelwrapper.h \
    filephotoprovider.h \
    urlphotoprovider.h \
    urlphotodialog.h \
    patientsearchedit.h \
    patientcore.h \
    patientdataextractordialog.h

SOURCES = patientbaseplugin.cpp \
    patientbase.cpp \
    patientmodel.cpp \
    patientselector.cpp \
    patientwidgetmanager.cpp \
    patientsearchmode.cpp \
    patientbasepreferencespage.cpp \
    patientbar.cpp \
    patientcreatorwizard.cpp \
    identityviewerwidget.cpp \
    patientmodelwrapper.cpp \
    filephotoprovider.cpp \
    urlphotoprovider.cpp \
    urlphotodialog.cpp \
    patientsearchedit.cpp \
    patientcore.cpp \
    patientdataextractordialog.cpp

FORMS += \
    patientselector.ui \
    patientbasepreferencespage.ui \
    patientbar.ui \
    identityviewer.ui \
    urlphotodialog.ui \
    patientdataextractordialog.ui

OTHER_FILES = PatientBase.pluginspec

# include translations
TRANSLATION_NAME = patientbase
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

RESOURCES += \
    patientbaseplugin.qrc
