TEMPLATE        = lib
TARGET          = TextEditor

DEFINES += EDITOR_LIBRARY
DEFINES += FREEPPORT
FREEPORT = 1

BUILD_PATH_POSTFIXE = FreePort

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/texteditorplugin/texteditor_sources.pri)

HEADERS += texteditorplugin.h \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.h

SOURCES += texteditorplugin.cpp \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.cpp

OTHER_FILES += TextEditor.pluginspec
