TEMPLATE        = lib

QT += webenginewidgets

TARGET          = PMH

DEFINES += PMH_LIBRARY

include(../fmf_plugins.pri)

!with-pmh{
    error(PMH plugin not requested)
} else {
    message(Building PMH plugin)
}

include(pmhplugin_dependencies.pri)

HEADERS = pmhplugin.h \
    pmh_exporter.h \
    pmhmode.h \
    pmhcategorymodel.h \
    pmhbase.h \
    constants.h \
    pmhviewer.h \
    pmhdata.h \
    pmhcreatordialog.h \
    pmhcore.h \
    pmhepisodemodel.h \
    pmhepisodeviewer.h \
    pmhwidgetmanager.h \
    pmhcontextualwidget.h \
    pmhpreferencespage.h \
    pmhcontentexporter.h \
    pmhtokens.h \
    previewpage.h
#    pmhmodel.h \
#    pmhcategorydialog.h \
#    pmhcategoryonlyproxymodel.h \
#    categorylabelsmodel.h \


SOURCES = pmhplugin.cpp \
    pmhmode.cpp \
    pmhcategorymodel.cpp \
    pmhbase.cpp \
    constants.cpp \
    pmhviewer.cpp \
    pmhdata.cpp \
    pmhcreatordialog.cpp \
    pmhcore.cpp \
    pmhepisodemodel.cpp \
    pmhepisodeviewer.cpp \
    pmhwidgetmanager.cpp \
    pmhcontextualwidget.cpp \
    pmhpreferencespage.cpp \
    pmhcontentexporter.cpp \
    pmhtokens.cpp \
    previewpage.cpp
#    pmhmodel.cpp \
#    pmhcategorydialog.cpp \
#    pmhcategoryonlyproxymodel.cpp \
#    categorylabelsmodel.cpp \

FORMS += \
    pmhviewer.ui \
    pmhmodewidget.ui \
    pmhcreatordialog.ui \
    pmhepisodeviewer.ui \
    pmhpreferencespage.ui
#    pmhcategorywidget.ui \

OTHER_FILES = PMH.pluginspec

# include translations
TRANSLATION_NAME = pmh
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
