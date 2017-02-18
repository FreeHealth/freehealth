TARGET = Feedback
TEMPLATE = lib

DEFINES += FEEDBACK_LIBRARY
BUILD_PATH_POSTFIXE = FreeHealth

INCLUDEPATH += ../

include(../fmf_plugins.pri)
include(feedback_dependencies.pri)

!with-feedback{
    error(Feedback plugin not requested)
} else {
    message(Building Feedback plugin)
}

HEADERS += \
    feedbackplugin.h\
    feedback_exporter.h\
    feedbackconstants.h \
    bugreportdialog.h

SOURCES += \
    feedbackplugin.cpp \
    bugreportdialog.cpp

OTHER_FILES = Feedback.pluginspec

#FREEMEDFORMS_SOURCES=%FreeMedFormsSources%
#IDE_BUILD_TREE=%FreeMedFormsBuild%

#include translations
TRANSLATION_NAME = feedback
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

FORMS += \
    bugreportdialog.ui

