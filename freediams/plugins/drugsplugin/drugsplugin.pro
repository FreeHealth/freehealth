TEMPLATE        = lib
TARGET          = Drugs

DEFINES += FREEDIAMS
FREEDIAMS = 1
with-pad:DEFINES+=WITH_PAD

BUILD_PATH_POSTFIXE = FreeDiams

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/drugsplugin/shared_sources.pri)

HEADERS += ../../../plugins/drugsplugin/drugsplugin.h \
#../../../plugins/drugsplugin/drugswidgetfactory.h

SOURCES += ../../../plugins/drugsplugin/drugsplugin.cpp \
#../../../plugins/drugsplugin/drugswidgetfactory.cpp

OTHER_FILES = Drugs.pluginspec
