TEMPLATE        = lib
TARGET          = Drugs
PACKAGE_VERSION = 0.0.9

DEFINES += DRUGS_LIBRARY
DEFINES += FREEDIAMS
FREEDIAMS = 1

include(../../../plugins/fmf_plugins.pri)
include(../../../plugins/drugsplugin/drugsplugin_dependencies.pri )

OTHER_FILES = Drugs.pluginspec

include(../../../plugins/drugsplugin/drugswidget.pri)

HEADERS += ../../../plugins/drugsplugin/drugsplugin.h \
#../../../plugins/drugsplugin/drugswidgetfactory.h

SOURCES += ../../../plugins/drugsplugin/drugsplugin.cpp \
#../../../plugins/drugsplugin/drugswidgetfactory.cpp
