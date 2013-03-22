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
    pdftkwrapper.h \
    chequeprinter.h \
    chequeprinter_preferences.h \
    chequeprinterdialog.h \
    fspprinter.h
        
SOURCES += \
    toolsplugin.cpp \
    toolspreferences.cpp \
    pdftkwrapper.cpp \
    chequeprinter.cpp \
    chequeprinter_preferences.cpp \
    chequeprinterdialog.cpp \
    fspprinter.cpp

FORMS += \
    toolspreferences.ui \
    chequeprinter_preferences.ui \
    chequeprinterdialog.ui

OTHER_FILES = Tools.pluginspec

#FREEMEDFORMS_SOURCES=%FreeMedFormsSources%
#IDE_BUILD_TREE=%FreeMedFormsBuild%

PROVIDER = FreeMedForms

# include translations
TRANSLATION_NAME = tools
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

