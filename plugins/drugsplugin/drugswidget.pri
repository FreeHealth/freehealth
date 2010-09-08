# This file includes common sources between
# DrugsWidget plugins and freediams
INCLUDEPATH *= $${PWD}

# General
HEADERS += $${PWD}/drugs_exporter.h \
    $${PWD}/constants.h \
    $${PWD}/drugswidgetmanager.h \
    $${PWD}/drugswidget/atctreeviewer.h
SOURCES += $${PWD}/drugswidgetmanager.cpp \
    $${PWD}/drugswidget/atctreeviewer.cpp

# Dosage Dialog
HEADERS += $${PWD}/dosagedialog/mfDosageDialog.h \
    $${PWD}/dosagedialog/mfDosageCreatorDialog.h \
    $${PWD}/dosagedialog/mfDosageViewer.h
SOURCES += $${PWD}/dosagedialog/mfDosageDialog.cpp \
    $${PWD}/dosagedialog/mfDosageCreatorDialog.cpp \
    $${PWD}/dosagedialog/mfDosageViewer.cpp
FORMS += $${PWD}/dosagedialog/mfDosageDialog.ui \
    $${PWD}/dosagedialog/mfDosageCreatorDialog.ui \
    $${PWD}/dosagedialog/mfDosageViewer.ui

# DrugsWidget Preferences
HEADERS += $${PWD}/drugspreferences/mfDrugsPreferences.h \
    $${PWD}/drugspreferences/databaseselectorwidget.h \
    $${PWD}/drugspreferences/protocolpreferencespage.h
SOURCES += $${PWD}/drugspreferences/mfDrugsPreferences.cpp \
    $${PWD}/drugspreferences/databaseselectorwidget.cpp \
    $${PWD}/drugspreferences/protocolpreferencespage.cpp
FORMS += $${PWD}/drugspreferences/drugsextraoptionspage.ui \
    $${PWD}/drugspreferences/drugsviewoptionspage.ui \
    $${PWD}/drugspreferences/drugsuseroptionspage.ui \
    $${PWD}/drugspreferences/drugsprintoptionspage.ui \
    $${PWD}/drugspreferences/databaseselectorwidget.ui \
    $${PWD}/drugspreferences/drugselectorwidget.ui \
    $${PWD}/drugspreferences/protocolpreferencespage.ui

# DrugsWidgets
HEADERS += $${PWD}/drugswidget/druginfo.h \
    $${PWD}/drugswidget/druginfo_p.h \
    $${PWD}/drugswidget/mfDrugSelector.h \
    $${PWD}/drugswidget/mfPrescriptionViewer.h \
    $${PWD}/drugswidget/mfDrugsCentralWidget.h \
    $${PWD}/drugswidget/textualprescriptiondialog.h \
    $${PWD}/drugswidget/dailyschemeviewer.h \
    $${PWD}/drugswidget/interactionsynthesisdialog.h
SOURCES += $${PWD}/drugswidget/druginfo.cpp \
    $${PWD}/drugswidget/mfDrugSelector.cpp \
    $${PWD}/drugswidget/mfPrescriptionViewer.cpp \
    $${PWD}/drugswidget/mfDrugsCentralWidget.cpp \
    $${PWD}/drugswidget/textualprescriptiondialog.cpp \
    $${PWD}/drugswidget/dailyschemeviewer.cpp \
    $${PWD}/drugswidget/interactionsynthesisdialog.cpp
FORMS += $${PWD}/drugswidget/druginfo.ui \
    $${PWD}/drugswidget/mfDrugSelector.ui \
    $${PWD}/drugswidget/mfPrescriptionViewer.ui \
    $${PWD}/drugswidget/mfDrugsCentralWidget.ui \
    $${PWD}/drugswidget/textualprescriptiondialog.ui \
    $${PWD}/drugswidget/dailyschemeviewer.ui \
    $${PWD}/drugswidget/interactionsynthesisdialog.ui

# translations
TRANSLATIONS += $${SOURCES_TRANSLATIONS}/mfDrugsWidget_fr.ts \
    $${SOURCES_TRANSLATIONS}/mfDrugsWidget_de.ts \
    $${SOURCES_TRANSLATIONS}/mfDrugsWidget_es.ts
