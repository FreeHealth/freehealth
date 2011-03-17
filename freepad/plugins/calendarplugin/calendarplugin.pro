TEMPLATE        = lib
TARGET          = Calendar

DEFINES += CALENDAR_LIBRARY
DEFINES += FREEPAD
FREEPAD = 1

BUILD_PATH_POSTFIXE = FreePad

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/calendarplugin/calendar_sources.pri)

OTHER_FILES += Calendar.pluginspec
