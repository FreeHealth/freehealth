TEMPLATE = lib
TARGET = Printer

DEFINES += PRINTER_LIBRARY

QT *= printsupport

include($${PWD}/../fmf_plugins.pri)
include( $${PWD}/printerplugin_dependencies.pri )

HEADERS = $${PWD}/printerplugin.h \
    $${PWD}/printer_exporter.h \
    $${PWD}/printer.h \
    $${PWD}/printerpreviewer_p.h \
    $${PWD}/textdocumentextra.h \ # textdocumentheader.h
    $${PWD}/printerpreferences.h \
    $${PWD}/documentprinter.h \
    $${PWD}/printdialog.h \
    $${PWD}/printcorrectionpreferences.h

SOURCES = $${PWD}/printerplugin.cpp \
    $${PWD}/printer.cpp \
    $${PWD}/printerpreviewer_p.cpp \
    $${PWD}/textdocumentextra.cpp \
    $${PWD}/printerpreferences.cpp \
    $${PWD}/documentprinter.cpp \
    $${PWD}/printdialog.cpp \
    $${PWD}/printcorrectionpreferences.cpp

FORMS = $${PWD}/printerpreviewer_p.ui \
    $${PWD}/printerpreferences.ui \
    $${PWD}/printdialog.ui \
    $${PWD}/printcorrectionpreferences.ui

# include translations
TRANSLATION_NAME = printer
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

OTHER_FILES = Printer.pluginspec
