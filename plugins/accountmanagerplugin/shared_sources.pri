DEFINES += ACCOUNTMANAGER_LIBRARY

include(../fmf_plugins.pri)
include(accountmanagerplugin_dependencies.pri)

HEADERS += $${PWD}/accountmanagerplugin.h \
    $${PWD}/accountmanager_exporter.h \
    $${PWD}/accountmanager.h

SOURCES += $${PWD}/accountmanagerplugin.cpp \
    $${PWD}/accountmanager.cpp

FORMS += $${PWD}/accountmanager.ui


TRANSLATIONS += $${SOURCES_TRANSLATIONS}/accountmanagerplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/accountmanagerplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/accountmanagerplugin_es.ts
