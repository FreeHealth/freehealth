TEMPLATE        = lib
TARGET          = TextEditor
PACKAGE_VERSION = 0.0.9

DEFINES += EDITOR_LIBRARY
DEFINES += FREEDIAMS
FREEDIAMS = 1

BUILD_PATH_POSTFIXE = FreeDiams

include(../../../plugins/fmf_plugins.pri)
include(../../../plugins/texteditorplugin/texteditorplugin_dependencies.pri )

OTHER_FILES = TextEditor.pluginspec

include(../../../plugins/texteditorplugin/shared_sources.pri)

HEADERS += texteditorplugin.h \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.h

SOURCES += texteditorplugin.cpp \
#../../../plugins/texteditorplugin/texteditorwidgetfactory.cpp
