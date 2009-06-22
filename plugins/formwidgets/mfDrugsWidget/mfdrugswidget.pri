# This file includes common sources between
# mfDrugsWidget plugins and DrugsInteractions
# $${PWD}/mfDrugsWidget.h \ <-- Specific to plugins
HEADERS += $${PWD}/mfDrugsConstants.h \
    $${PWD}/drugsdatabase/mfDrugsBase.h \
    $${PWD}/drugsmodel/mfDrugs.h \
    $${PWD}/drugsmodel/mfDrugInteraction.h \
    $${PWD}/drugswidget/mfDrugInfo.h \
    $${PWD}/drugswidget/mfDrugInfo_p.h \
    $${PWD}/drugsmodel/mfDrugsModel.h \
    $${PWD}/drugswidget/mfDrugSelector.h \
    $${PWD}/drugswidget/mfDosageDialog.h \
    $${PWD}/drugsmodel/mfDosageModel.h \
    $${PWD}/drugswidget/mfDosageCreatorDialog.h \
    $${PWD}/drugswidget/mfPrescriptionViewer.h \
    $${PWD}/drugswidget/mfDrugsPreferences.h \
    $${PWD}/drugswidget/mfInteractionDialog.h \
    $${PWD}/drugswidget/mfDosageAvailableWidget.h

# $${PWD}/mfDrugsWidget.cpp \ <-- Specific to plugins
SOURCES += $${PWD}/drugsdatabase/mfDrugsBase.cpp \
    $${PWD}/drugsmodel/mfDrugs.cpp \
    $${PWD}/drugsmodel/mfDrugInteraction.cpp \
    $${PWD}/drugswidget/mfDrugInfo.cpp \
    $${PWD}/drugsmodel/mfDrugsModel.cpp \
    $${PWD}/drugswidget/mfDrugSelector.cpp \
    $${PWD}/drugswidget/mfDosageDialog.cpp \
    $${PWD}/drugsmodel/mfDosageModel.cpp \
    $${PWD}/drugswidget/mfDosageCreatorDialog.cpp \
    $${PWD}/drugswidget/mfPrescriptionViewer.cpp \
    $${PWD}/drugswidget/mfDrugsPreferences.cpp \
    $${PWD}/drugswidget/mfInteractionDialog.cpp \
    $${PWD}/drugswidget/mfDosageAvailableWidget.cpp
FORMS += $${PWD}/drugswidget/mfDrugInfo.ui \
    $${PWD}/drugswidget/mfDrugSelector.ui \
    $${PWD}/drugswidget/mfDosageDialog.ui \
    $${PWD}/drugswidget/mfDosageCreatorDialog.ui \
    $${PWD}/drugswidget/mfPrescriptionViewer.ui \
    $${PWD}/drugswidget/mfDrugsPreferences.ui \
    $${PWD}/drugswidget/mfInteractionDialog.ui \
    $${PWD}/drugswidget/mfDosageAvailableWidget.ui
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/mfDrugsWidget_fr.ts
exists( $${PROTECTED_PATH} ):RESOURCES += $${PROTECTED_PATH}/drugs/drugsresources.qrc
else:RESOURCES += $${PWD}/widget_resources.qrc
