TEMPLATE        = lib
TARGET          = TextEditor

with-pad:DEFINES+=WITH_PAD
DEFINES *= FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/texteditorplugin/texteditor_sources.pri)

HEADERS += texteditorplugin.h \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.h

SOURCES += texteditorplugin.cpp \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.cpp

OTHER_FILES = TextEditor.pluginspec
