# SOURCES and HEADERS needed by the printer engine
SOURCES += $${PWD}/tkPrinterPreviewer_p.cpp \
           $${PWD}/tkTextDocumentExtra.cpp

HEADERS += $${PWD}/tkPrinter.h \
           $${PWD}/tkPrinterPreviewer_p.h \
           $${PWD}/tkTextDocumentExtra.h

FORMS   += $${PWD}/tkPrinterPreviewer_p.ui

# protected files
exists( $${PROTECTED_PATH} ):SOURCES += $${PROTECTED_PATH}/toolkit/objects/tkPrinter_Pro.cpp
else:SOURCES += $${PWD}/tkPrinter.cpp
