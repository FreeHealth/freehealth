TARGET = Webcam
TEMPLATE = lib

BUILD_PATH_POSTFIXE = FreeMedForms
PROVIDER = FreeMedForms

DEFINES += WEBCAM_LIBRARY

include(../fmf_plugins.pri)
include(webcam_dependencies.pri)

HEADERS = \
   webcamplugin.h \
   webcam_exporter.h \
   webcamconstants.h \
   webcamphotoprovider.h \
   webcamdialog.h \
   trackcontroller.h \
   processingthread.h \
   imagebuffer.h \
   headstate.h \
   filter.h \
   capturethread.h \
   renderwidget.h \
    webcamdialog_timerbased.h

SOURCES += \
   webcamplugin.cpp \
   webcamphotoprovider.cpp \
   webcamdialog.cpp \
   trackcontroller.cpp \
   processingthread.cpp \
   capturethread.cpp \
   filter.cpp \
   renderwidget.cpp \
   imagebuffer.cpp \
    webcamdialog_timerbased.cpp

FORMS += \
   webcamdialog.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/webcam_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_es.ts

OTHER_FILES = \
    Webcam.pluginspec

