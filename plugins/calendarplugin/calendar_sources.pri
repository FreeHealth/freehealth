DEFINES += PADTOOLS_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/calendarplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/calendarplugin.h \
    $${PWD}/calendar_exporter.h \
    $${PWD}/calendarimpl.h \
    $${PWD}/calendar_widget.h \
    $${PWD}/view.h \
    $${PWD}/day_range_view.h \
    $${PWD}/month_view.h \
    $${PWD}/calendar_item.h \
    $${PWD}/calendar_navbar.h \
    $${PWD}/common.h

SOURCES += \
    $${PWD}/calendarplugin.cpp \
    $${PWD}/calendarimpl.cpp \
    $${PWD}/calendar_widget.cpp \
    $${PWD}/view.cpp \
    $${PWD}/day_range_view.cpp \
    $${PWD}/month_view.cpp \
    $${PWD}/calendar_item.cpp \
    $${PWD}/calendar_navbar.cpp \
    $${PWD}/common.cpp

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/calendarplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/calendarplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/calendarplugin_es.ts
