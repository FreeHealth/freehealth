TEMPLATE = lib
TARGET = Printer

with-pad:DEFINES+=WITH_PAD
DEFINES *= FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/printerplugin/printer_sources.pri)

OTHER_FILES += Printer.pluginspec
