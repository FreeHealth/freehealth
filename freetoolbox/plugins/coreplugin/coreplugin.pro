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
    pathpreferences.h \
    ifullreleasestep.h \
    fullreleasepage.h

SOURCES += coreimpl.cpp \
    appaboutpage.cpp \
    coreplugin.cpp \
    mainwindow.cpp \
    pathpreferences.cpp \
    ifullreleasestep.cpp \
    fullreleasepage.cpp

FORMS += mainwindow.ui \
    pathpreferences.ui \
    fullreleasepage.ui

OTHER_FILES += Core.pluginspec

# include translations
TRANSLATION_NAME = ftbcore
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)


