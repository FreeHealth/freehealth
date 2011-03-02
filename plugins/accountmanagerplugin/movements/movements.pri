include( $${PWD}/../../accountbaseplugin/basemovements.pri)

HEADERS += $${PWD}/movementsviewer.h \
    $${PWD}/movementsIO.h \
    $${PWD}/movementsmanager.h

FORMS += $${PWD}/movementsviewer.ui

SOURCES += $${PWD}/movementsviewer.cpp \
    $${PWD}/movementsIO.cpp \
    $${PWD}/movementsmanager.cpp

#CONFIG += no_lflags_merge
