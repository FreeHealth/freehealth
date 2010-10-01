TEMPLATE = lib
TARGET = FormManager
PACKAGE_VERSION = 0.0.2
DEFINES += FORMMANAGER_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( formmanagerplugin_dependencies.pri )
HEADERS = formmanagerplugin.h \
    formmanager_exporter.h \
    formmanager.h \
    iformio.h \
    iformitem.h \
    iformitemspec.h \
    iformitemscripts.h \
    iformitemvalues.h \
    iformwidgetfactory.h \
    iformitemdata.h \
    formmanagermode.h \
    formplaceholder.h \
    episodemodel.h \
    episodebase.h \
    constants_db.h \
    formfilesselectorwidget.h \
    formmanagerpreferencespage.h
SOURCES = formmanagerplugin.cpp \
    formmanager.cpp \
    iformitem.cpp \
    iformwidgetfactory.cpp \
    formmanagermode.cpp \
    formplaceholder.cpp \
    episodemodel.cpp \
    episodebase.cpp \
    formfilesselectorwidget.cpp \
    formmanagerpreferencespage.cpp
OTHER_FILES = FormManager.pluginspec
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/formmanagerplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/formmanagerplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/formmanagerplugin_es.ts
FORMS += formfilesselectorwidget.ui
