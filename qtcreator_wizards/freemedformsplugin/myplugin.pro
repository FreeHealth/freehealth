TARGET = %PluginName%
TEMPLATE = lib

DEFINES += %PluginName:u%_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

# %PluginName% files

include(../fmf_plugins.pri)
include( %PluginName:l%_dependencies.pri )

SOURCES += %PluginName:l%plugin.cpp

HEADERS += %PluginName:l%plugin.h\
        %PluginName:l%_exporter.h\
        %PluginName:l%constants.h

OTHER_FILES = %PluginName%.pluginspec


# FreeMedForms linking

#FREEMEDFORMS_SOURCES=%FreeMedFormsSources%
#IDE_BUILD_TREE=%FreeMedFormsBuild%

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\FreeMedForms" on Windows Vista and later
##    "$XDG_DATA_HOME/freemedforms" or "~/.local/share/freemedforms" on Linux
##    "~/Library/Application Support/FreeMedForms" on Mac

## Destination directory is not used in FMF 
# %DestDir%USE_USER_DESTDIR = yes

PROVIDER = %VendorName%

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/%PluginName:l%_es.ts
