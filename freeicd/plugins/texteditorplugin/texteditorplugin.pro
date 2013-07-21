TEMPLATE        = lib
TARGET          = TextEditor

DEFINES += EDITOR_LIBRARY
INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/texteditorplugin/shared_sources.pri)

HEADERS += texteditorplugin.h \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.h

SOURCES += texteditorplugin.cpp \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.cpp

OTHER_FILES += TextEditor.pluginspec
