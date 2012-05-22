DEFINES += WEBCAM_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/webcam_dependencies.pri)

HEADERS += \
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
    $${PWD}/capturethread.cpp \
    $${PWD}/renderwidget.cpp

FORMS += \
    $${PWD}/webcamdialog.ui

TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/webcam_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/webcam_es.ts
