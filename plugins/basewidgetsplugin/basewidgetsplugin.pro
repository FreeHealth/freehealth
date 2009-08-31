TEMPLATE        = lib
TARGET          = BaseWidgets
PACKAGE_VERSION = 0.0.2

DEFINES += BASEFORMWIDGETS_LIBRARY

include(../fmf_plugins.pri)
include( basewidgetsplugin_dependencies.pri )

HEADERS = \
          basewidgetsplugin.h \
          baseformwidgets.h \
baseformwidgetsoptionspage.h \
#          baseformwidgetsaboutpage.h
#basewigets_exporter.h \

SOURCES = basewidgetsplugin.cpp \
          baseformwidgets.cpp \
baseformwidgetsoptionspage.cpp \
#          baseformwidgetsaboutpage.cpp

FORMS += baseformwidgetsoptionspage.ui

OTHER_FILES = BaseWidgets.pluginspec
