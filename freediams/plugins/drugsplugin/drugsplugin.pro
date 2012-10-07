TEMPLATE        = lib
TARGET          = Drugs

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/drugsplugin/shared_sources.pri)

HEADERS += ../../../plugins/drugsplugin/drugsplugin.h \
#../../../plugins/drugsplugin/drugswidgetfactory.h

SOURCES += ../../../plugins/drugsplugin/drugsplugin.cpp \
#../../../plugins/drugsplugin/drugswidgetfactory.cpp

OTHER_FILES = Drugs.pluginspec
