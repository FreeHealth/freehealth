TEMPLATE = lib
TARGET = BaseWidgets
PACKAGE_VERSION = 0.0.2
DEFINES += BASEFORMWIDGETS_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( basewidgetsplugin_dependencies.pri )
HEADERS += basewidgetsplugin.h \
    baseformwidgets.h \
    baseformwidgetsoptionspage.h \
    texteditorfactory.h \
    identitywidgetfactory.h

# baseformwidgetsaboutpage.h
# basewigets_exporter.h \
SOURCES += basewidgetsplugin.cpp \
    baseformwidgets.cpp \
    baseformwidgetsoptionspage.cpp \
    texteditorfactory.cpp \
    identitywidgetfactory.cpp

# baseformwidgetsaboutpage.cpp
FORMS += baseformwidgetsoptionspage.ui \
    baseformwidget.ui
OTHER_FILES = BaseWidgets.pluginspec
