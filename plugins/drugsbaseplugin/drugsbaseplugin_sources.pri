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
    $${PWD}/drugsdatabaseaboutpage.h \
    $${PWD}/drugstemplateprinter.h \
    $${PWD}/drugsdatabaseselector.h
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
    $${PWD}/drugsdatabaseaboutpage.cpp \
    $${PWD}/drugstemplateprinter.cpp \
    $${PWD}/drugsdatabaseselector.cpp

# Translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/drugsbaseplugin_fr.ts \
    $${SOURCES_TRANSLATIONS}/drugsbaseplugin_de.ts \
    $${SOURCES_TRANSLATIONS}/drugsbaseplugin_es.ts

# protected
exists( $${SOURCES_PROTECTED_PATH} ):RESOURCES += $${SOURCES_PROTECTED_PATH}/drugs/drugsresources.qrc
else:RESOURCES += $${PWD}/resources.qrc
