TEMPLATE = lib
TARGET = Core
INCLUDEPATH += ../
DEPENDPATH += ../

include(../../../plugins/coreplugin/common_core.pri)

QT *= sql network xml

HEADERS += coreimpl.h \
    appaboutpage.h \
    coreplugin.h \
    mainwindow.h \
    itoolpage.h \
    ftb_constants.h \
    globaltools.h \
    pathpreferences.h \
    ifullreleasestep.h \
    fullreleasepage.h \
    routesmodel.h \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.h \
    drugdatabasedescription.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp \
    mainwindow.cpp \
    globaltools.cpp \
    pathpreferences.cpp \
    ifullreleasestep.cpp \
    fullreleasepage.cpp \
    routesmodel.cpp \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \
    drugdatabasedescription.cpp

FORMS += mainwindow.ui \
    pathpreferences.ui \
    fullreleasepage.ui

OTHER_FILES += Core.pluginspec


