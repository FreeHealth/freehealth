DEFINES += PADTOOLS_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/padtoolsplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/padtools_exporter.h \
    $${PWD}/pad_analyzer.h \
    $${PWD}/padtoolsimpl.h \
    $${PWD}/pad_highlighter.h \
	$${PWD}/pad.h \
	$${PWD}/pad_fragment.h \
	$${PWD}/pad_string.h \
	$${PWD}/pad_core.h

SOURCES += \
    $${PWD}/pad_analyzer.cpp \
    $${PWD}/padtoolsimpl.cpp \
    $${PWD}/pad_highlighter.cpp \
	$${PWD}/pad.cpp \
	$${PWD}/pad_fragment.cpp \
	$${PWD}/pad_string.cpp \
	$${PWD}/pad_core.cpp

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
