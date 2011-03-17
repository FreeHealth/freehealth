DEFINES += PADTOOLS_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/calendarplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/calendarplugin.h \
    $${PWD}/calendar_exporter.h \
    $${PWD}/calendarimpl.h \
    $${PWD}/calendar_widget.h

SOURCES += \
    $${PWD}/calendarplugin.cpp \
    $${PWD}/calendarimpl.cpp \
    $${PWD}/calendar_widget.cpp

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/calendarplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/calendarplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/calendarplugin_es.ts
