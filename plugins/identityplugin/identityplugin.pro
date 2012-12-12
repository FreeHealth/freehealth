TARGET = Identity
TEMPLATE = lib

DEFINES += IDENTITY_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(identity_dependencies.pri)

HEADERS += \
    identityplugin.h \
    identity_exporter.h \
    identityconstants.h \
#    identitypreferences.h \
    identityeditorwidget.h \
    themedgenderbutton.h
        
SOURCES += \
    identityplugin.cpp \
#    identitypreferences.cpp \
    identityeditorwidget.cpp \
    themedgenderbutton.cpp

FORMS += \
    identitypreferences.ui \
    identityeditorwidget.ui

OTHER_FILES = Identity.pluginspec

PROVIDER = FreeMedForms

# include translations
TRANSLATION_NAME = identity
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

