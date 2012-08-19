TEMPLATE        = lib
TARGET          = PMH

DEFINES += PMH_LIBRARY
DEFINES += FREEMEDFORMS
with-pad { DEFINES += WITH_PAD }

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
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
    pmhpreferencespage.h
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
    pmhpreferencespage.cpp
#    pmhmodel.cpp \
#    pmhcategorydialog.cpp \
#    pmhcategoryonlyproxymodel.cpp \
#    categorylabelsmodel.cpp \


OTHER_FILES = PMH.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/pmhplugin_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/pmhplugin_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/pmhplugin_es.ts

FORMS += \
    pmhviewer.ui \
    pmhmodewidget.ui \
    pmhcreatordialog.ui \
    pmhepisodeviewer.ui \
    pmhpreferencespage.ui
#    pmhcategorywidget.ui \
