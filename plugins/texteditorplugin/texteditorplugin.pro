TEMPLATE        = lib
TARGET          = TextEditor
PACKAGE_VERSION = 0.0.2

DEFINES += EDITOR_LIBRARY

include(../fmf_plugins.pri)
include( texteditorplugin_dependencies.pri )

HEADERS = texteditorplugin.h \
    texteditor_exporter.h \
    editoractionhandler.h \
    editorcontext.h \
    editormanager.h \
    tabledialog.h \
    tableeditor.h \
    texteditor.h \


SOURCES = texteditorplugin.cpp \
    editoractionhandler.cpp \
    editormanager.cpp \
    tableeditor.cpp \
    texteditor.cpp \


FORMS = tabledialog.ui

TRANSLATIONS += $${SOURCES_GLOBAL_RESOURCES}/translations/texteditorplugin_fr.ts \
                $${SOURCES_GLOBAL_RESOURCES}/translations/texteditorplugin_de.ts

OTHER_FILES = TextEditor.pluginspec
