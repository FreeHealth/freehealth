TEMPLATE = lib
TARGET = TextEditor

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../../plugins/texteditorplugin/shared_sources.pri)

HEADERS += texteditorplugin.h
SOURCES += texteditorplugin.cpp

OTHER_FILES += TextEditor.pluginspec
