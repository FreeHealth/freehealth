DEFINES += PADTOOLS_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/calendarplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/calendarplugin.h \
    $${PWD}/calendar_exporter.h \
    $${PWD}/calendarimpl.h \
    $${PWD}/calendar_widget.h \
    $${PWD}/day_view.h \
    $${PWD}/week_view.h \
    $${PWD}/month_view.h

SOURCES += \
    $${PWD}/calendarplugin.cpp \
    $${PWD}/calendarimpl.cpp \
    $${PWD}/calendar_widget.cpp \
    $${PWD}/day_view.cpp \
    $${PWD}/week_view.cpp \
    $${PWD}/month_view.cpp

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/calendarplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/calendarplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/calendarplugin_es.ts
