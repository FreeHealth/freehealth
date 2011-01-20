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
    pmhbase.h \
    constants.h \
    pmhviewer.h


SOURCES = pmhplugin.cpp \
    pmhmode.cpp \
    pmhmodel.cpp \
    pmhbase.cpp \
    constants.cpp \
    pmhviewer.cpp


OTHER_FILES = PMH.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS}/pmhplugin_fr.ts \
                $${SOURCES_TRANSLATIONS}/pmhplugin_de.ts \
                $${SOURCES_TRANSLATIONS}/pmhplugin_es.ts

FORMS += \
    pmhviewer.ui \
    pmhmode.ui
