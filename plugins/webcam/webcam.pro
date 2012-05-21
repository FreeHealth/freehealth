TARGET = Webcam
TEMPLATE = lib
LIBS +=-lopencv_core -lopencv_highgui

DEFINES += WEBCAM_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms
PROVIDER = FreeMedForms

include(../fmf_plugins.pri)
include(webcam_dependencies.pri)

HEADERS += \
    webcamplugin.h \
    webcam_exporter.h \
    webcamconstants.h \
    webcamphotoprovider.h \
    webcamdialog.h \
    renderwidget.h \
    headstate.h \
    filter.h \
    capturethread.h \
    imagebuffer.h \
    trackcontroller.h \
    processingthread.h
        
SOURCES += \
    webcamplugin.cpp \
    webcamphotoprovider.cpp \
    capturethread.cpp \
    webcamdialog.cpp \
    renderwidget.cpp \
    filter.cpp \
    trackcontroller.cpp \
    processingthread.cpp

OTHER_FILES = Webcam.pluginspec

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/webcam_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_es.ts

FORMS += \
    webcamdialog.ui

