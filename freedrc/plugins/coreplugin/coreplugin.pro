TEMPLATE = lib
TARGET = Core

include(../../../plugins/coreplugin/common_core.pri)
include(coreplugin_dependencies.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

# specific derivative files
HEADERS += coreplugin.h \
    coreimpl.h \
    appaboutpage.h \
    commandlineparser.h \
    patient.h \
    freeaccount_constants.h

SOURCES += coreplugin.cpp \
    coreimpl.cpp \
    appaboutpage.cpp \
    commandlineparser.cpp \
    patient.cpp

# include translations
TRANSLATION_NAME = facore
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES += $${PWD}/Core.pluginspec
