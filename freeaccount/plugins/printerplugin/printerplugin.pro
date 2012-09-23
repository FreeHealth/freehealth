TEMPLATE = lib
TARGET = Printer

INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/printerplugin/printer_sources.pri)

OTHER_FILES += Printer.pluginspec
