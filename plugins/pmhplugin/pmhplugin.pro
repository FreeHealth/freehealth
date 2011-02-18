TEMPLATE        = lib
TARGET          = PMH

DEFINES += PMH_LIBRARY
DEFINES += FREEMEDFORMS

include(../fmf_plugins.pri)
include(pmhplugin_dependencies.pri)

HEADERS = pmhplugin.h \
    pmh_exporter.h \
    pmhmode.h \
    pmhmodel.h \
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
    pmhcategorydialog.h \
    pmhcategoryonlyproxymodel.h \
    categorylabelsmodel.h \
    pmhpreferencespage.h


SOURCES = pmhplugin.cpp \
    pmhmode.cpp \
    pmhmodel.cpp \
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
    pmhcategorydialog.cpp \
    pmhcategoryonlyproxymodel.cpp \
    categorylabelsmodel.cpp \
    pmhpreferencespage.cpp


OTHER_FILES = PMH.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/pmhplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/pmhplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/pmhplugin_es.ts

FORMS += \
    pmhviewer.ui \
    pmhmodewidget.ui \
    pmhcreatordialog.ui \
    pmhepisodeviewer.ui \
    pmhcategorywidget.ui \
    pmhpreferencespage.ui
