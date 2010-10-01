TEMPLATE = lib
TARGET = Drugs
PACKAGE_VERSION = 0.0.9

DEFINES += DRUGS_LIBRARY
DEFINES += FREEMEDFORMS
BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(drugsplugin_dependencies.pri)

OTHER_FILES = Drugs.pluginspec

CONFIG *= plugins

QT *= sql network

# include DrugsWidgets sources
include(drugswidget.pri)

# include FreeMedForms specific sources
HEADERS += drugsplugin.h \
    drugswidgetfactory.h

SOURCES += drugsplugin.cpp \
    drugswidgetfactory.cpp

