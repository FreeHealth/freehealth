TEMPLATE = lib
TARGET = TextEditor
PACKAGE_VERSION = 0.0.2

DEFINES += EDITOR_LIBRARY

include(../fmf_plugins.pri)
include(texteditorplugin_dependencies.pri)

include(texteditor_sources.pri)

HEADERS += texteditorplugin.h texteditorfactory.h
SOURCES += texteditorplugin.cpp texteditorfactory.cpp

OTHER_FILES += TextEditor.pluginspec
