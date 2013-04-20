TARGET = Tools
TEMPLATE = lib

# Manage Qt >= 5
greaterThan(QT_MAJOR_VERSION, 4): QT *= printsupport

DEFINES += TOOLS_LIBRARY
BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(tools_dependencies.pri)

HEADERS += \
    toolsplugin.h \
    tools_exporter.h \
    toolsconstants.h \
    toolspreferences.h \
    pdftkwrapper.h \
    cheque/chequeprinter.h \
    cheque/chequeprinter_preferences.h \
    cheque/chequeprinterdialog.h \
    fsp/fsp.h \
    fsp/fspprinter.h \
    fsp/fspprinterdialog.h \
    fsp/fspprinterpreferences.h \
    fsp/fspconstants.h \
    fsp/fsptemplatemodel.h \
    cheque/chequeconstants.h \
    constants.h \
    cheque/chequeprintformat.h \
    cheque/chequeprintformatmodel.h


SOURCES += \
    toolsplugin.cpp \
    toolspreferences.cpp \
    pdftkwrapper.cpp \
    cheque/chequeprinter.cpp \
    cheque/chequeprinter_preferences.cpp \
    cheque/chequeprinterdialog.cpp \
    fsp/fsp.cpp \
    fsp/fspprinter.cpp \
    fsp/fspprinterdialog.cpp \
    fsp/fspprinterpreferences.cpp \
    fsp/fsptemplatemodel.cpp \
    cheque/chequeprintformat.cpp \
    cheque/chequeprintformatmodel.cpp


FORMS += \
    toolspreferences.ui \
    cheque/chequeprinter_preferences.ui \
    cheque/chequeprinterdialog.ui \
    fsp/fspprinterdialog.ui \
    fsp/fspprinterdialog_patient.ui \
    fsp/fspprinterdialog_conds.ui \
    fsp/fspprinterdialog_fees.ui \
    fsp/fspprinterdialog_prerecorded.ui \
    fsp/fspprinterpreferences.ui

OTHER_FILES = Tools.pluginspec

PROVIDER = FreeMedForms

# include translations
TRANSLATION_NAME = tools
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

