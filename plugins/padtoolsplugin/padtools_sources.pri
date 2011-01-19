DEFINES += PADTOOLS_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/padtoolsplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/padtools_exporter.h

#SOURCES += \
#    $${PWD}/editoractionhandler.cpp \
#    $${PWD}/editormanager.cpp \
#    $${PWD}/tableeditor.cpp \
#    $${PWD}/texteditor.cpp \
#    $${PWD}/tablepropertieswidget.cpp

#FORMS += $${PWD}/tabledialog.ui \
#    $${PWD}/tablepropertieswidget.ui \
#    $${PWD}/tablepropertiesdialog.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/padtoolsplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/padtoolsplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/padtoolsplugin_es.ts
