include( $${PWD}/../../accountbaseplugin/basemovements.pri)

HEADERS += $${PWD}/ledgerIO.h \
    $${PWD}/ledgermanager.h \
    $${PWD}/ledgerviewer.h \
    $${PWD}/ledgeredit.h \
    $${PWD}/mythread.h

FORMS += $${PWD}/ledgerviewer.ui \
    $${PWD}/ledgeredit.ui

SOURCES += $${PWD}/ledgerIO.cpp \
    $${PWD}/ledgermanager.cpp \
    $${PWD}/ledgerviewer.cpp \
    $${PWD}/ledgeredit.cpp \
    $${PWD}/mythread.cpp
