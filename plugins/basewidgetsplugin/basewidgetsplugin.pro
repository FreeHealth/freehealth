TEMPLATE = lib
TARGET = BaseWidgets

DEFINES += BASEFORMWIDGETS_LIBRARY

BUILD_PATH_POSTFIXE = FreeMedForms

include(../fmf_plugins.pri)
include( basewidgetsplugin_dependencies.pri )
HEADERS += basewidgetsplugin.h \
    baseformwidgets.h \
    baseformwidgetsoptionspage.h \
    texteditorfactory.h \
    identitywidgetfactory.h \
#    qtuiformwidgets.h
# baseformwidgetsaboutpage.h
# basewigets_exporter.h \
    frenchsocialnumber.h \
    calculationwidgets.h


SOURCES += basewidgetsplugin.cpp \
    baseformwidgets.cpp \
    baseformwidgetsoptionspage.cpp \
    texteditorfactory.cpp \
    identitywidgetfactory.cpp \
#    qtuiformwidgets.cpp
# baseformwidgetsaboutpage.cpp
    frenchsocialnumber.cpp \
    calculationwidgets.cpp


FORMS += baseformwidgetsoptionspage.ui \
    baseformwidget.ui \
    frenchsocialnumber.ui


OTHER_FILES = BaseWidgets.pluginspec


