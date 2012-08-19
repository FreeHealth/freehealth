TEMPLATE = lib
TARGET = Core
PACKAGE_VERSION = 0.0.2

with-pad:DEFINES+=WITH_PAD
BUILD_PATH_POSTFIXE = FreeAccount

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

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/faccountcoreplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/faccountcoreplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/faccountcoreplugin_es.ts

OTHER_FILES += $${PWD}/Core.pluginspec
