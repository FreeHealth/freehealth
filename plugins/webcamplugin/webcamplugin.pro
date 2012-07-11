TARGET = Webcam
TEMPLATE = lib

BUILD_PATH_POSTFIXE = FreeMedForms
PROVIDER = FreeMedForms

include(shared_sources.pri)

OTHER_FILES = \
    Webcam.pluginspec

HEADERS = \
    $${PWD}/webcamplugin.h \
    $${PWD}/webcam_exporter.h \
    $${PWD}/webcamconstants.h \
    $${PWD}/webcamphotoprovider.h \
    $${PWD}/webcamdialog.h \
    $${PWD}/trackcontroller.h \
    $${PWD}/processingthread.h \
    $${PWD}/imagebuffer.h \
    $${PWD}/headstate.h \
    $${PWD}/filter.h \
    $${PWD}/capturethread.h \
    $${PWD}/renderwidget.h

SOURCES += \
    $${PWD}/webcamplugin.cpp \
    $${PWD}/webcamphotoprovider.cpp \
    $${PWD}/webcamdialog.cpp \
    $${PWD}/trackcontroller.cpp \
    $${PWD}/processingthread.cpp \
    $${PWD}/capturethread.cpp \
    $${PWD}/filter.cpp \
    $${PWD}/renderwidget.cpp

FORMS += \
    $${PWD}/webcamdialog.ui
