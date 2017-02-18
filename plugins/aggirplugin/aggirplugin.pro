TEMPLATE = lib
TARGET = Gir

DEFINES += AGGIR_LIBRARY

BUILD_PATH_POSTFIXE = FreeHealth

include(../fmf_plugins.pri)
include( aggirplugin_dependencies.pri )
HEADERS = \
    aggirplugin.h \
    aggir_exporter.h \
    girwidget.h \
    girmodel.h

SOURCES = \
    aggirplugin.cpp \
    girwidget.cpp \
    girmodel.cpp

FORMS += girwidget.ui

OTHER_FILES = Gir.pluginspec

# include translations
TRANSLATION_NAME = aggir
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
