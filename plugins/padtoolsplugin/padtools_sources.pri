DEFINES += PADTOOLS_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/padtoolsplugin_dependencies.pri )

!with-pad{
    error(PadTools plugin not requested)
} else {
    message(Building PadTools plugin)
}

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
    $${PWD}/tokenpool.h \
    $${PWD}/padtoolscore.h \
    $${PWD}/padtoolscontextualwidgetmanager.h

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
    $${PWD}/tokenpool.cpp \
    $${PWD}/padtoolscore.cpp \
    $${PWD}/padtoolscontextualwidgetmanager.cpp

FORMS += \
    $${PWD}/padwriter.ui \
    $${PWD}/tokeneditorwidget.ui \
    $${PWD}/tokeneditor.ui

# include translations
TRANSLATION_NAME = padtools
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
