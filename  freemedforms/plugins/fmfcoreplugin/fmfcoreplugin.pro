TEMPLATE        = lib
TARGET          = Core
PACKAGE_VERSION = 0.0.2

# include shared files for the coreplugin.
# FreeMedForms and derivatives share the same Core basic structure that is
# found in an unique pri file. The files may be included from <coreplugin/...> instead of
# <fmfcoreplugin/...>
include(../coreplugin/common_core.pri)

DEFINES += FREEMEDFORMS

QT *= script

# specific derivative files
HEADERS += \
    coreplugin.h \
    coreimpl.h \
    commandlineparser.h \
    appaboutpage.h

SOURCES += \
    coreplugin.cpp \
    coreimpl.cpp \
#    mainwindow.cpp \
    appaboutpage.cpp

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/fmfcoreplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/fmfcoreplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/fmfcoreplugin_es.ts


OTHER_FILES = Core.pluginspec
