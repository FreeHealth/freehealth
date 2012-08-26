DEFINES += PADTOOLS_LIBRARY
message(Building PadTools plugin)

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/padtoolsplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/padtoolsplugin.h \
    $${PWD}/padtools_exporter.h \
    $${PWD}/pad_analyzer.h \
    $${PWD}/padtoolsimpl.h \
    $${PWD}/pad_highlighter.h \
    $${PWD}/pad_document.h \
    $${PWD}/pad_fragment.h \
    $${PWD}/pad_string.h \
    $${PWD}/pad_item.h \
    $${PWD}/tokenmodel.h \
    $${PWD}/padwriter.h \
    $${PWD}/constants.h \
    $${PWD}/dragdroptextedit.h \
    $${PWD}/tokeneditorwidget.h \
    $${PWD}/tokeneditor.h \
    $${PWD}/tokenoutputdocument.h \
    $${PWD}/tokentreeview.h \
    $${PWD}/tokenpool.h

SOURCES += \
    $${PWD}/padtoolsplugin.cpp \
    $${PWD}/pad_analyzer.cpp \
    $${PWD}/padtoolsimpl.cpp \
    $${PWD}/pad_highlighter.cpp \
    $${PWD}/pad_document.cpp \
    $${PWD}/pad_fragment.cpp \
    $${PWD}/pad_string.cpp \
    $${PWD}/pad_item.cpp \
    $${PWD}/tokenmodel.cpp \
    $${PWD}/padwriter.cpp \
    $${PWD}/dragdroptextedit.cpp \
    $${PWD}/tokeneditorwidget.cpp \
    $${PWD}/tokeneditor.cpp \
    $${PWD}/tokenoutputdocument.cpp \
    $${PWD}/tokentreeview.cpp \
    $${PWD}/tokenpool.cpp

FORMS += \
    $${PWD}/padwriter.ui \
    $${PWD}/tokeneditorwidget.ui \
    $${PWD}/tokeneditor.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/padtoolsplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/padtoolsplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/padtoolsplugin_es.ts

