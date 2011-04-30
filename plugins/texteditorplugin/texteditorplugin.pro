TEMPLATE = lib
TARGET = TextEditor

BUILD_PATH_POSTFIXE = FreeMedForms

include(texteditor_sources.pri)

HEADERS += texteditorplugin.h
SOURCES += texteditorplugin.cpp

OTHER_FILES += TextEditor.pluginspec
