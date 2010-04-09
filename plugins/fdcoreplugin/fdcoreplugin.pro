TEMPLATE = lib
TARGET = Core
PACKAGE_VERSION = 0.0.2

# include shared files for the coreplugin.
# FreeMedForms and derivatives share the same Core basic structure that is
# found in an unique pri file. The files may be included from <coreplugin/...> instead of
# <fdcoreplugin/...>
include(../coreplugin/common_core.pri)
include(fdcoreplugin_dependencies.pri)

# specific derivative files
HEADERS += coreplugin.h \
    coreimpl.h \ # mainwindow.h \
    appaboutpage.h \
    commandlineparser.h \
    patient.h

SOURCES += coreplugin.cpp \
    coreimpl.cpp \ # mainwindow.cpp \
    appaboutpage.cpp \
    patient.cpp \
    commandlineparser.cpp

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/fdcoreplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/fdcoreplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/fdcoreplugin_es.ts
OTHER_FILES += $${PWD}/Core.pluginspec
