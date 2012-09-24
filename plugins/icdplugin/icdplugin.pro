TEMPLATE = lib
TARGET = ICD

include(icdplugin_sources.pri)

OTHER_FILES = ICD.pluginspec

HEADERS += \
    icdwidgetfactory.h

SOURCES += \
    icdwidgetfactory.cpp
