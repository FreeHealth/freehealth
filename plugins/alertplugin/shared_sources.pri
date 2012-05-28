TEMPLATE = lib
TARGET = Alert

DEFINES += ALERT_LIBRARY

include(../fmf_plugins.pri)
include(alertplugin_dependencies.pri)

HEADERS += \
    $${PWD}/constants.h \
    $${PWD}/alertplugin.h \
    $${PWD}/alertbase.h \
    $${PWD}/alertcore.h \
    $${PWD}/alertmanager.h \
    $${PWD}/ialert.h \
    $${PWD}/showalertsIHM.h \
    $${PWD}/xmlalert.h

SOURCES += \
    $${PWD}/alertplugin.cpp \
    $${PWD}/alertbase.cpp \
    $${PWD}/alertcore.cpp \
    $${PWD}/alertmanager.cpp \
    $${PWD}/ialert.cpp \
#    $${PWD}/main.cpp \
    $${PWD}/showalertsIHM.cpp \
    $${PWD}/xmlalert.cpp

FORMS += $${PWD}/ShowAlertsWidget.ui

OTHER_FILES += $${PWD}/Alert.pluginspec

TRANSLATIONS += \
    $${SOURCES_TRANSLATIONS_PATH}/alertplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/alertplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/alertplugin_es.ts
