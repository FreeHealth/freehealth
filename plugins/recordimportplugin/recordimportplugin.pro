TARGET = recordimport
TEMPLATE = lib

DEFINES += RECORDIMPORT_LIBRARY
BUILD_PATH_POSTFIXE = FreeHealth

INCLUDEPATH += ../

include(../fmf_plugins.pri)
include(recordimport_dependencies.pri)

!with-recordimport{
    error(recordimport plugin not requested)
} else {
    message(Building recordimport plugin)
}

HEADERS += \
    recordimportplugin.h\
    recordimport_exporter.h\
    recordimportconstants.h \
    recordimportdialog.h \
    processrecordimport.h \
    recordimportdialog_p.h \
    processrecordimport_p.h
        
SOURCES += \
    recordimportplugin.cpp \
    recordimportdialog.cpp \
    processrecordimport.cpp

OTHER_FILES = recordimport.pluginspec

#equals(TEST, 1) {
#    SOURCES += \
#        tests/test_recordimportplugin.cpp
#}

#include translations
TRANSLATION_NAME = recordimport
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

FORMS += \
    recordimportdialog.ui
