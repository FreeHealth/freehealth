TEMPLATE = lib
TARGET = Drugs

DEFINES += DRUGS_LIBRARY
DEFINES += FREEMEDFORMS
BUILD_PATH_POSTFIXE = FreeMedForms

# include DrugsWidgets sources
include(shared_sources.pri)

# include FreeMedForms specific sources
HEADERS += drugsplugin.h \
    drugswidgetfactory.h

SOURCES += drugsplugin.cpp \
    drugswidgetfactory.cpp

OTHER_FILES = Drugs.pluginspec
