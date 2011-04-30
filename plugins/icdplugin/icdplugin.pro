TEMPLATE        = lib
TARGET          = ICD

DEFINES += ICD_LIBRARY
DEFINES += FREEMEDFORMS
FREEMEDFORMS = 1

include(../fmf_plugins.pri)
include(icdplugin_dependencies.pri)
include(icdplugin_sources.pri)

OTHER_FILES = ICD.pluginspec

HEADERS += \
    icdwidgetfactory.h

SOURCES += \
    icdwidgetfactory.cpp
