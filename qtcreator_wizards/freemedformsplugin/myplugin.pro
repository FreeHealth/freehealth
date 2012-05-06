TARGET = %PluginName%
TEMPLATE = lib

DEFINES += %PluginName:u%_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms

# %PluginName% files
HEADERS += %PluginName:l%plugin.h\
        %PluginName:l%_exporter.h\
        %PluginName:l%constants.h
        
SOURCES += %PluginName:l%plugin.cpp


OTHER_FILES = %PluginName%.pluginspec


# FreeMedForms linking

include(../fmf_plugins.pri)

#FREEMEDFORMS_SOURCES=%FreeMedFormsSources%
#IDE_BUILD_TREE=%FreeMedFormsBuild%

PROVIDER = %VendorName%

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_es.ts
