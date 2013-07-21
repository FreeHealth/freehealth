TEMPLATE = lib
TARGET = Biblio
DEFINE += BIBLIO_LIBRARY

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
    biblio_exporter.h \
    biblioplugin.h \
    bibliocore.h \
    bibliobase.h

OTHER_FILES += Biblio.pluginspec




