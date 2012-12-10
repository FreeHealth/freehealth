TEMPLATE        = lib
TARGET          = Script

DEFINES += SCRIPT_LIBRARY

include(../fmf_plugins.pri)
include( scriptplugin_dependencies.pri )

HEADERS = scriptplugin.h script_exporter.h \
    scriptwrappers.h \
    scriptmanager.h \
    scriptpatientwrapper.h \
    uitools.h \
    tools.h

SOURCES = scriptplugin.cpp \
    scriptwrappers.cpp \
    scriptmanager.cpp \
    scriptpatientwrapper.cpp \
    uitools.cpp \
    tools.cpp

OTHER_FILES = Script.pluginspec \
    $${SOURCES_GLOBAL_RESOURCES}/test.js

# include translations
TRANSLATION_NAME = script
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
