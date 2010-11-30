TEMPLATE = lib
TARGET = TextEditor
PACKAGE_VERSION = 0.0.2

BUILD_PATH_POSTFIXE = FreeMedForms

include(texteditor_sources.pri)

HEADERS += texteditorplugin.h
SOURCES += texteditorplugin.cpp

OTHER_FILES += TextEditor.pluginspec
