TEMPLATE = lib
TARGET = Biblio

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include(biblio_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    biblioplugin.cpp \
    bibliocore.cpp \
    bibliobase.cpp


HEADERS  += \
    biblioplugin.h \
    bibliocore.h \
    bibliobase.h

OTHER_FILES += Biblio.pluginspec




