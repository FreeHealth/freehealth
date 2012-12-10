TEMPLATE = lib
TARGET = Gir

DEFINES += AGGIR_LIBRARY
with-pad { DEFINES += WITH_PAD }

BUILD_PATH_POSTFIXE = FreeMedForms

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
