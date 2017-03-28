TEMPLATE = lib
TARGET = Core

# include shared files for the coreplugin.
# FreeHealth and derivatives share the same Core basic structure that is
# found in a unique pri file. The files may be included from <coreplugin/...> instead of
# <fmfcoreplugin/...>
include(../coreplugin/common_core.pri)

# specific derivative files
HEADERS += coreplugin.h \
    coreimpl.h \
    commandlineparser.h \
    appaboutpage.h \
    appconfigwizard.h \
    sqlitedatabasepathpreferences.h

SOURCES += coreplugin.cpp \
    coreimpl.cpp \
    appaboutpage.cpp \
    appconfigwizard.cpp \
    commandlineparser.cpp \
    sqlitedatabasepathpreferences.cpp

# include translations
TRANSLATION_NAME = fmfcore
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES = Core.pluginspec

FORMS += \
    sqlitedatabasepathpreferences.ui
