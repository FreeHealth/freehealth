# This file includes common sources between
# DrugsWidget plugins and freediams
# $${PWD}/mfDrugsWidget.h \ <-- Specific to plugins
INCLUDEPATH *= $${PWD}

include( $${PWD}/dosagedialog/dosagedialog.pri)
include( $${PWD}/drugsdatabase/drugsdatabase.pri)
include( $${PWD}/drugspreferences/drugspreferences.pri)
include( $${PWD}/drugsmodel/drugsmodel.pri)
include( $${PWD}/interactionsmodel/interactionsmodel.pri)
include( $${PWD}/drugswidget/drugswidget.pri)

HEADERS += $${PWD}/drugs_exporter.h \
	   $${PWD}/mfDrugsConstants.h \
	   $${PWD}/mfDrugsManager.h

SOURCES += $${PWD}/mfDrugsManager.cpp

# translations
TRANSLATIONS += $${SOURCES_GLOBAL_RESOURCES}/translations/mfDrugsWidget_fr.ts
                $${SOURCES_GLOBAL_RESOURCES}/translations/mfDrugsWidget_de.ts


exists( $${SOURCES_PROTECTED_PATH} ):RESOURCES += $${SOURCES_PROTECTED_PATH}/drugs/drugsresources.qrc
else:RESOURCES += $${PWD}/widget_resources.qrc
