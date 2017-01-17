TARGET = Webcam
TEMPLATE = lib

BUILD_PATH_POSTFIXE = FreeMedForms

DEFINES += WEBCAM_LIBRARY

include(../fmf_plugins.pri)

!with-webcam{
    error(WebCam plugin not requested)
} else {
    message(Building WebCam plugin)
}

include(webcam_dependencies.pri)
include($${SOURCES_BUILDSPECS_PATH}/config_opencv.pri)

HEADERS = \
    webcamplugin.h \
    webcam_exporter.h \
    webcamconstants.h \
    webcamphotoprovider.h \
    opencvwidget.h \
    webcamdialog.h \
    webcampreferences.h

SOURCES += \
    webcamplugin.cpp \
    webcamphotoprovider.cpp \
    opencvwidget.cpp \
    webcamdialog.cpp \
    webcampreferences.cpp

FORMS += \
    webcamdialog.ui \
    webcampreferences.ui

OTHER_FILES = \
    Webcam.pluginspec

# include translations
TRANSLATION_NAME = webcam
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
