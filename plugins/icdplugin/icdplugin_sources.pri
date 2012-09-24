DEFINES += ICD_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/icdplugin_dependencies.pri)

HEADERS += $${PWD}/icdplugin.h \
    $${PWD}/icd_exporter.h \
    $${PWD}/icddatabase.h \
    $${PWD}/icddownloader.h \
    $${PWD}/icdwidgetmanager.h \
    $${PWD}/icdcontextualwidget.h \
    $${PWD}/icdcodeselector.h \
    $${PWD}/simpleicdmodel.h \
    $${PWD}/constants.h \
    $${PWD}/icdviewer.h \
    $${PWD}/fullicdcodemodel.h \
    $${PWD}/icdcollectionmodel.h \
    $${PWD}/icdassociation.h \
    $${PWD}/icddialog.h \
    $${PWD}/icdcentralwidget.h \
    $${PWD}/icdio.h \
    $${PWD}/icdsearchmodel.h \
    $${PWD}/icdcollectiondialog.h

SOURCES += $${PWD}/icdplugin.cpp \
    $${PWD}/icddatabase.cpp \
    $${PWD}/icddownloader.cpp \
    $${PWD}/icdwidgetmanager.cpp \
    $${PWD}/icdcontextualwidget.cpp \
    $${PWD}/icdcodeselector.cpp \
    $${PWD}/simpleicdmodel.cpp \
    $${PWD}/icdviewer.cpp \
    $${PWD}/fullicdcodemodel.cpp \
    $${PWD}/icdcollectionmodel.cpp \
    $${PWD}/icdassociation.cpp \
    $${PWD}/icddialog.cpp \
    $${PWD}/icdcentralwidget.cpp \
    $${PWD}/icdio.cpp \
    $${PWD}/icdsearchmodel.cpp \
    $${PWD}/icdcollectiondialog.cpp

FORMS += $${PWD}/icdviewer.ui \
    $${PWD}/icdcodeselector.ui \
    $${PWD}/icdcentralwidget.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/icdplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/icdplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/icdplugin_es.ts

OTHER_FILES += $${SOURCES_RESOURCES_SQL}/icd10/icd10.sql
