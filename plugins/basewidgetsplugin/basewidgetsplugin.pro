TEMPLATE = lib
TARGET = BaseWidgets

DEFINES += BASEFORMWIDGETS_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include(basewidgetsplugin_dependencies.pri)

HEADERS += basewidgetsplugin.h \
    constants.h \
    baseformwidgets.h \
    baseformwidgetsoptionspage.h \
    texteditorfactory.h \
    identitywidgetfactory.h \
    calculationwidgets.h \
    frenchsocialnumber.h \
    basedetailswidget.h \
    basedatecompleterwidget.h \
    baselistwidgets.h \
    frenchsocialnumberwidget.h \
    measurementwidget.h \
    treeviewformitem.h \
    buttonmenupathitemview.h
#    basezipcodeswidget.h

SOURCES += basewidgetsplugin.cpp \
    constants.cpp \
    baseformwidgets.cpp \
    baseformwidgetsoptionspage.cpp \
    texteditorfactory.cpp \
    identitywidgetfactory.cpp \
    frenchsocialnumber.cpp \
    calculationwidgets.cpp \
    basedetailswidget.cpp \
    basedatecompleterwidget.cpp \
    baselistwidgets.cpp \
    frenchsocialnumberwidget.cpp \
    measurementwidget.cpp \
    treeviewformitem.cpp \
    buttonmenupathitemview.cpp
#    basezipcodeswidget.cpp

FORMS += baseformwidgetsoptionspage.ui \
    baseformwidget.ui \
    frenchsocialnumber.ui \
    austriansocialnumber.ui

OTHER_FILES = BaseWidgets.pluginspec

# include translations
TRANSLATION_NAME = basewidgets
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

