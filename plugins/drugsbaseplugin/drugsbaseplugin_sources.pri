DEFINES += DRUGSBASE_LIBRARY
include($${PWD}/../fmf_plugins.pri)
include($${PWD}/drugsbaseplugin_dependencies.pri )
HEADERS += $${PWD}/drugsbaseplugin.h \
    $${PWD}/drugsbase_exporter.h \
    $${PWD}/drugsbase.h \
    $${PWD}/interactionsbase.h \
    $${PWD}/drugsdata.h \
    $${PWD}/drugsinteraction.h \
    $${PWD}/interactionsmanager.h \
    $${PWD}/constants.h \
    $${PWD}/drugsmodel.h \
    $${PWD}/dosagemodel.h \
    $${PWD}/drugsio.h \
    $${PWD}/drugsbasemanager.h \
    $${PWD}/globaldrugsmodel.h \
    $${PWD}/dailyschememodel.h \
    $${PWD}/versionupdater.h \
    $${PWD}/drugstemplateprinter.h \
    $${PWD}/drugsdatabaseselector.h \
    $${PWD}/atctreemodel.h \
    $${PWD}/drugsearchengine.h

#    $${PWD}/drugsdatabaseaboutpage.h \

SOURCES += $${PWD}/drugsbaseplugin.cpp \
    $${PWD}/drugsbase.cpp \
    $${PWD}/interactionsbase.cpp \
    $${PWD}/drugsdata.cpp \
    $${PWD}/drugsinteraction.cpp \
    $${PWD}/interactionsmanager.cpp \
    $${PWD}/drugsmodel.cpp \
    $${PWD}/dosagemodel.cpp \
    $${PWD}/drugsio.cpp \
    $${PWD}/drugsbasemanager.cpp \
    $${PWD}/globaldrugsmodel.cpp \
    $${PWD}/dailyschememodel.cpp \
    $${PWD}/versionupdater.cpp \
    $${PWD}/drugstemplateprinter.cpp \
    $${PWD}/drugsdatabaseselector.cpp \
    $${PWD}/atctreemodel.cpp \
    $${PWD}/drugsearchengine.cpp

#    $${PWD}/drugsdatabaseaboutpage.cpp \


# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/drugsbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/drugsbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/drugsbaseplugin_es.ts

# /** \todo TO BE REMOVED */
OTHER_FILES = \
    $${SOURCES_PROTECTED_PATH}/afssaps_db/bin/resources/processed/link-class-substances.csv \
    $${SOURCES_PROTECTED_PATH}/afssaps_db/bin/resources/processed/inns_molecules_link.csv \

# protected
exists( $${SOURCES_PROTECTED_PATH} ):RESOURCES += $${SOURCES_PROTECTED_PATH}/drugs/drugsresources.qrc
else:RESOURCES += $${PWD}/resources.qrc
