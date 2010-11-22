TEMPLATE        = lib
TARGET          = TextEditor
PACKAGE_VERSION = 0.0.9

DEFINES += EDITOR_LIBRARY
DEFINES += FREEICD
FREEICD = 1

BUILD_PATH_POSTFIXE = FreeICD

include(../../../plugins/fmf_plugins.pri)
include(../../../plugins/texteditorplugin/texteditorplugin_dependencies.pri )

OTHER_FILES = TextEditor.pluginspec

include(../../../plugins/texteditorplugin/texteditor_sources.pri)

HEADERS += texteditorplugin.h \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.h

SOURCES += texteditorplugin.cpp \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.cpp
