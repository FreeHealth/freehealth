TEMPLATE = lib
TARGET = Printer

DEFINES *= FREEACCOUNT
FREEACCOUNT = 1

BUILD_PATH_POSTFIXE = FreeAccount

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/printerplugin/printer_sources.pri)

OTHER_FILES += Printer.pluginspec
