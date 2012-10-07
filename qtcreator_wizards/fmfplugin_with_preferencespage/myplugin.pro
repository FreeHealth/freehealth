TARGET = %PluginName%
TEMPLATE = lib

DEFINES += %PluginName:u%_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(%PluginName:l%_dependencies.pri)

HEADERS += \
    %PluginName:l%plugin.h \
    %PluginName:l%_exporter.h \
    %PluginName:l%constants.h \
    %PluginName:l%preferences.h
        
SOURCES += \
    %PluginName:l%plugin.cpp \
    %PluginName:l%preferences.cpp

FORMS += \
    %PluginName:l%preferences.ui

OTHER_FILES = %PluginName%.pluginspec

#FREEMEDFORMS_SOURCES=%FreeMedFormsSources%
#IDE_BUILD_TREE=%FreeMedFormsBuild%

PROVIDER = %VendorName%

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_es.ts
