TEMPLATE = lib
TARGET = FormManager

DEFINES += FORMMANAGER_LIBRARY

BUILD_PATH_POSTFIXE = FreeHealth

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
    constants_settings.h \
    formfilesselectorwidget.h \
    formmanagerpreferencespage.h \
    formcontextualwidget.h \
    formeditordialog.h \
    firstrunformmanager.h \
    formiodescription.h \
    formioquery.h \
    subforminsertionpoint.h \
    episodedata.h \
    formtreemodel.h \
    formdatawidgetmapper.h \
    formcore.h \
    formcollection.h \
    formcontextualwidgetmanager.h \
    episodemanager.h \
    formviewdelegate.h \
    patientformitemdatawrapper.h \
    formexporter.h \
    formitemtoken.h

SOURCES = formmanagerplugin.cpp \
    formmanager.cpp \
    iformitem.cpp \
    iformitemdata.cpp \
    iformwidgetfactory.cpp \
    formmanagermode.cpp \
    formplaceholder.cpp \
    episodemodel.cpp \
    episodebase.cpp \
    formfilesselectorwidget.cpp \
    formmanagerpreferencespage.cpp \
    iformio.cpp \
    formcontextualwidget.cpp \
    formeditordialog.cpp \
    firstrunformmanager.cpp \
    episodedata.cpp \
    formtreemodel.cpp \
    formdatawidgetmapper.cpp \
    formcore.cpp \
    formcollection.cpp \
    formcontextualwidgetmanager.cpp \
    subforminsertpoint.cpp \
    episodemanager.cpp \
    formviewdelegate.cpp \
    patientformitemdatawrapper.cpp \
    formexporter.cpp \
    formitemtoken.cpp

FORMS += formfilesselectorwidget.ui \
    formeditordialog.ui \
    formmanagerpreferenceswidget.ui \
    formpreferenceswidget.ui \
    formplaceholder.ui

OTHER_FILES = FormManager.pluginspec

equals(TEST, 1){
    SOURCES += \
        tests/tst_formitem.cpp \
        tests/tst_formmanager.cpp
}

# include translations
TRANSLATION_NAME = formmanager
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

RESOURCES += \
    resources.qrc \
    formmanagerplugin.qrc
