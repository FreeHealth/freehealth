TEMPLATE = lib
TARGET = Core
PACKAGE_VERSION = 0.0.2

BUILD_PATH_POSTFIXE = FreeAccount


# include shared files for the coreplugin.
# FreeMedForms and derivatives share the same Core basic structure that is
# found in an unique pri file. The files may be included from <coreplugin/...> instead of
# <fdcoreplugin/...>
include(../coreplugin/common_core.pri)
include(faccountcoreplugin_dependencies.pri)

# specific derivative files
HEADERS += coreplugin.h \
    coreimpl.h \
    appaboutpage.h \
    commandlineparser.h

SOURCES += coreplugin.cpp \
    coreimpl.cpp \
    appaboutpage.cpp \
    commandlineparser.cpp

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/faccountcoreplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/faccountcoreplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/faccountcoreplugin_es.ts

OTHER_FILES += $${PWD}/Core.pluginspec
