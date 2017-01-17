TARGET = Tools
TEMPLATE = lib

QT *= printsupport

DEFINES += TOOLS_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(tools_dependencies.pri)

# French FSP Feature
contains(DEFINES, WITH_FRENCH_FSP){
    HEADERS += \
        fsp/fsp.h \
        fsp/fspprinter.h \
        fsp/fspprinterdialog.h \
        fsp/fspprinterpreferences.h \
        fsp/fspconstants.h \
        fsp/fsptemplatemodel.h

    SOURCES += \
        fsp/fsp.cpp \
        fsp/fspprinter.cpp \
        fsp/fspprinterdialog.cpp \
        fsp/fspprinterpreferences.cpp \
        fsp/fsptemplatemodel.cpp

    FORMS += \
        fsp/fspprinterdialog.ui \
        fsp/fspprinterdialog_patient.ui \
        fsp/fspprinterdialog_conds.ui \
        fsp/fspprinterdialog_fees.ui \
        fsp/fspprinterdialog_prerecorded.ui \
        fsp/fspprinterpreferences.ui
}

# Cheque printing Feature
contains(DEFINES, WITH_CHEQUE_PRINTING){
    HEADERS += \
        cheque/chequeconstants.h \
        cheque/chequeprinter.h \
        cheque/chequeprinter_preferences.h \
        cheque/chequeprinterdialog.h \
        cheque/chequeprintformat.h \
        cheque/chequeprintformatmodel.h

    SOURCES += \
        cheque/chequeprinter.cpp \
        cheque/chequeprinter_preferences.cpp \
        cheque/chequeprinterdialog.cpp \
        cheque/chequeprintformat.cpp \
        cheque/chequeprintformatmodel.cpp

    FORMS += \
        cheque/chequeprinter_preferences.ui \
        cheque/chequeprinterdialog.ui
}

# HPRIM integrator
contains(DEFINES, WITH_FRENCH_HPRIM_INTEGRATOR){
    QT *= xml

    HEADERS += \
        hprimintegrator/hprimintegrator.h \
        hprimintegrator/hprimfilemodel.h \
        hprimintegrator/hprimpreferences.h \
        hprimintegrator/constants.h
        #hprimintegrator/hprimintegratordialog.h

    SOURCES += \
        hprimintegrator/hprimintegrator.cpp \
        hprimintegrator/hprimfilemodel.cpp \
        hprimintegrator/hprimpreferences.cpp
        #hprimintegrator/hprimintegratordialog.cpp

    FORMS += \
        hprimintegrator/hprimintegratorwidget.ui \
        hprimintegrator/hprimpreferences.ui
        #hprimintegrator/hprimintegratordialog.ui
}


# General code
HEADERS += \
    constants.h \
    toolsplugin.h \
    tools_exporter.h \
    toolsconstants.h \
    toolspreferences.h \
    pdftkwrapper.h

SOURCES += \
    toolsplugin.cpp \
    toolspreferences.cpp \
    pdftkwrapper.cpp

FORMS += \
    toolspreferences.ui

OTHER_FILES = Tools.pluginspec

# include translations
TRANSLATION_NAME = tools
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
