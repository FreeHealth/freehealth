TARGET = patientimport
TEMPLATE = lib

DEFINES += PATIENTIMPORT_LIBRARY
BUILD_PATH_POSTFIXE = FreeHealth

INCLUDEPATH += ../

include(../fmf_plugins.pri)
include(patientimport_dependencies.pri)

!with-patientimport{
    error(patientimport plugin not requested)
} else {
    message(Building patientimport plugin)
}

HEADERS += \
    patientimportplugin.h\
    patientimport_exporter.h\
    patientimportconstants.h \
    patientimportdialog.h \
    processpatientimport.h \
    patientimportdialog_p.h \
    processpatientimport_p.h
        
SOURCES += \
    patientimportplugin.cpp \
    patientimportdialog.cpp \
    processpatientimport.cpp

OTHER_FILES = patientimport.pluginspec

#equals(TEST, 1) {
#    SOURCES += \
#        tests/test_patientimportplugin.cpp
#}

#include translations
TRANSLATION_NAME = patientimport
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

FORMS += \
    patientimportdialog.ui
