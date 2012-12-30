TARGET = Tools
TEMPLATE = lib

DEFINES += TOOLS_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(tools_dependencies.pri)

HEADERS += \
    toolsplugin.h \
    tools_exporter.h \
    toolsconstants.h \
    toolspreferences.h \
    pdftkwrapper.h
        
SOURCES += \
    toolsplugin.cpp \
    toolspreferences.cpp \
    pdftkwrapper.cpp

FORMS += \
    toolspreferences.ui

OTHER_FILES = Tools.pluginspec

#FREEMEDFORMS_SOURCES=%FreeMedFormsSources%
#IDE_BUILD_TREE=%FreeMedFormsBuild%

PROVIDER = FreeMedForms

# include translations
TRANSLATION_NAME = tools
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

