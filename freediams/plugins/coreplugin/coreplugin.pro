TEMPLATE = lib
TARGET = Core

DEFINES += FREEDIAMS
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

# include shared files for the coreplugin.
# FreeMedForms and derivatives share the same Core basic structure that is
# found in an unique pri file. The files may be included from <coreplugin/...> instead of
# <fdcoreplugin/...>
include(../../../plugins/coreplugin/common_core.pri)
include(coreplugin_dependencies.pri)

# specific derivative files
HEADERS += coreplugin.h \
    coreimpl.h \ # mainwindow.h \
    appaboutpage.h \
    commandlineparser.h \
    patient.h \
    user.h

SOURCES += coreplugin.cpp \
    coreimpl.cpp \ # mainwindow.cpp \
    appaboutpage.cpp \
    patient.cpp \
    commandlineparser.cpp \
    user.cpp

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/fdcoreplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/fdcoreplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/fdcoreplugin_es.ts

OTHER_FILES += Core.pluginspec
