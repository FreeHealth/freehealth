TEMPLATE = lib
TARGET = Core

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
    coreimpl.h \
    appaboutpage.h \
    commandlineparser.h \
    patient.h \
    user.h

SOURCES += coreplugin.cpp \
    coreimpl.cpp \
    appaboutpage.cpp \
    patient.cpp \
    commandlineparser.cpp \
    user.cpp

# include translations
TRANSLATION_NAME = fdcore
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)


OTHER_FILES += Core.pluginspec
