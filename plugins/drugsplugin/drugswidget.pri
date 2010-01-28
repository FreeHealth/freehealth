# This file includes common sources between
# DrugsWidget plugins and freediams
INCLUDEPATH *= $${PWD}

# General
HEADERS += $${PWD}/drugs_exporter.h \
    $${PWD}/constants.h \
    $${PWD}/drugswidgetmanager.h


SOURCES += $${PWD}/drugswidgetmanager.cpp


# Dosage Dialog
HEADERS += $${PWD}/dosagedialog/mfDosageDialog.h \
    $${PWD}/dosagedialog/mfDosageCreatorDialog.h \
    $${PWD}/dosagedialog/mfDosageViewer.h \
    $${PWD}/dosagedialog/mfDosageAvailableWidget.h
SOURCES += $${PWD}/dosagedialog/mfDosageDialog.cpp \
    $${PWD}/dosagedialog/mfDosageCreatorDialog.cpp \
    $${PWD}/dosagedialog/mfDosageViewer.cpp \
    $${PWD}/dosagedialog/mfDosageAvailableWidget.cpp
FORMS += $${PWD}/dosagedialog/mfDosageDialog.ui \
    $${PWD}/dosagedialog/mfDosageCreatorDialog.ui \
    $${PWD}/dosagedialog/mfDosageViewer.ui \
    $${PWD}/dosagedialog/mfDosageAvailableWidget.ui

# DrugsWidget Preferences
HEADERS += $${PWD}/drugspreferences/mfDrugsPreferences.h
SOURCES += $${PWD}/drugspreferences/mfDrugsPreferences.cpp
FORMS += $${PWD}/drugspreferences/drugsextraoptionspage.ui \
    $${PWD}/drugspreferences/drugsviewoptionspage.ui \
    $${PWD}/drugspreferences/drugsuseroptionspage.ui \
    $${PWD}/drugspreferences/drugsprintoptionspage.ui

# DrugsWidgets
HEADERS += $${PWD}/drugswidget/druginfo.h \
    $${PWD}/drugswidget/druginfo_p.h \
    $${PWD}/drugswidget/mfDrugSelector.h \
    $${PWD}/drugswidget/mfPrescriptionViewer.h \
    $${PWD}/drugswidget/mfInteractionDialog.h \
    $${PWD}/drugswidget/mfDrugsCentralWidget.h \
    $${PWD}/drugswidget/textualprescriptiondialog.h \
    $${PWD}/drugswidget/dailyschemeviewer.h

SOURCES += $${PWD}/drugswidget/druginfo.cpp \
    $${PWD}/drugswidget/mfDrugSelector.cpp \
    $${PWD}/drugswidget/mfPrescriptionViewer.cpp \
    $${PWD}/drugswidget/mfInteractionDialog.cpp \
    $${PWD}/drugswidget/mfDrugsCentralWidget.cpp \
    $${PWD}/drugswidget/textualprescriptiondialog.cpp \
    $${PWD}/drugswidget/dailyschemeviewer.cpp

FORMS += $${PWD}/drugswidget/druginfo.ui \
    $${PWD}/drugswidget/mfDrugSelector.ui \
    $${PWD}/drugswidget/mfPrescriptionViewer.ui \
    $${PWD}/drugswidget/mfInteractionDialog.ui \
    $${PWD}/drugswidget/mfDrugsCentralWidget.ui \
    $${PWD}/drugswidget/textualprescriptiondialog.ui \
    $${PWD}/drugswidget/dailyschemeviewer.ui

# translations
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/mfDrugsWidget_fr.ts \
                $${SOURCES_TRANSLATIONS}/mfDrugsWidget_de.ts \
                $${SOURCES_TRANSLATIONS}/mfDrugsWidget_es.ts
