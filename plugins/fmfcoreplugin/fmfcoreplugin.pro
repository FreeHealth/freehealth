TEMPLATE = lib
TARGET = Core

# include shared files for the coreplugin.
# FreeMedForms and derivatives share the same Core basic structure that is
# found in an unique pri file. The files may be included from <coreplugin/...> instead of
# <fmfcoreplugin/...>
include(../coreplugin/common_core.pri)

# specific derivative files
HEADERS += coreplugin.h \
    coreimpl.h \
    commandlineparser.h \
    appaboutpage.h \
    appconfigwizard.h

SOURCES += coreplugin.cpp \
    coreimpl.cpp \
    appaboutpage.cpp \
    appconfigwizard.cpp \
    commandlineparser.cpp

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/fmfcoreplugin_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/fmfcoreplugin_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/fmfcoreplugin_es.ts

OTHER_FILES = Core.pluginspec
