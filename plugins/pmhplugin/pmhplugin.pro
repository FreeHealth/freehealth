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
    pmhepisodeviewer.h


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
    pmhepisodeviewer.cpp


OTHER_FILES = PMH.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/pmhplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/pmhplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/pmhplugin_es.ts

FORMS += \
    pmhviewer.ui \
    pmhmode.ui \
    pmhcreatordialog.ui \
    pmhepisodeviewer.ui
