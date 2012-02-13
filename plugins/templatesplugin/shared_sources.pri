DEFINES += TEMPLATES_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/templatesplugin_dependencies.pri)

QT += sql

HEADERS += $${PWD}/templatesplugin.h \
    $${PWD}/templates_exporter.h \
    $${PWD}/itemplates.h \
    $${PWD}/itemplateprinter.h \
    $${PWD}/templatebase.h \
    $${PWD}/templatesmodel.h \
    $${PWD}/constants.h \
    $${PWD}/templatesview.h \
    $${PWD}/templatesview_p.h \
    $${PWD}/templateseditdialog.h \
    $${PWD}/templatespreferencespages.h \
    $${PWD}/templatescreationdialog.h \
    $${PWD}/templatescore.h

SOURCES += $${PWD}/templatesplugin.cpp \
    $${PWD}/templatebase.cpp \
    $${PWD}/templatesmodel.cpp \
    $${PWD}/templatesview.cpp \
    $${PWD}/templateseditdialog.cpp \
    $${PWD}/templatespreferencespages.cpp \
    $${PWD}/templatescreationdialog.cpp \
    $${PWD}/itemplates.cpp \
    $${PWD}/templatescore.cpp

FORMS += $${PWD}/templatesview.ui \
    $${PWD}/templateseditdialog.ui \
    $${PWD}/templatescontenteditor.ui \
    $${PWD}/templatespreferenceswidget.ui \
    $${PWD}/templatescreationdialog.ui

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/templatesplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/templatesplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/templatesplugin_es.ts
