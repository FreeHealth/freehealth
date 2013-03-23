TARGET = Tools
TEMPLATE = lib

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
    chequeprinter.h \
    chequeprinter_preferences.h \
    chequeprinterdialog.h \
    fsp/fsp.h \
    fsp/fspprinter.h \
    fsp/fspprinterdialog.h \
    fsp/fspprinterpreferences.h \
    fsp/fspconstants.h

        
SOURCES += \
    toolsplugin.cpp \
    toolspreferences.cpp \
    pdftkwrapper.cpp \
    chequeprinter.cpp \
    chequeprinter_preferences.cpp \
    chequeprinterdialog.cpp \
    fsp/fsp.cpp \
    fsp/fspprinter.cpp \
    fsp/fspprinterdialog.cpp \
    fsp/fspprinterpreferences.cpp


FORMS += \
    toolspreferences.ui \
    chequeprinter_preferences.ui \
    chequeprinterdialog.ui \
    fsp/fspprinterdialog.ui \
    fsp/fspprinterdialog_patient.ui \
    fsp/fspprinterdialog_conds.ui \
    fsp/fspprinterdialog_amounts.ui \
    fsp/fspprinterpreferences.ui

OTHER_FILES = Tools.pluginspec

PROVIDER = FreeMedForms

# include translations
TRANSLATION_NAME = tools
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

