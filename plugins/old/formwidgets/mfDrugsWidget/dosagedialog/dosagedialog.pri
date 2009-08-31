HEADERS += $${PWD}/mfDosageDialog.h \
           $${PWD}/mfDosageCreatorDialog.h \
           $${PWD}/mfDosageViewer.h \
           $${PWD}/mfDosageAvailableWidget.h

# $${PWD}/mfDrugsWidget.cpp \ <-- Specific to plugins
SOURCES += $${PWD}//mfDosageDialog.cpp \
           $${PWD}/mfDosageCreatorDialog.cpp \
           $${PWD}/mfDosageViewer.cpp \
           $${PWD}/mfDosageAvailableWidget.cpp

FORMS +=   $${PWD}/mfDosageDialog.ui \
           $${PWD}/mfDosageCreatorDialog.ui \
           $${PWD}/mfDosageViewer.ui \
           $${PWD}/mfDosageAvailableWidget.ui
