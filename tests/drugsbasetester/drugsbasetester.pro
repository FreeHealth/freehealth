TARGET = drugsbasetester
CONFIG += console
TEMPLATE = app

# include general configuration
include( ../../config.pri )

# include SDKs
include ( ../../plugins/sdk_drugswidget.pri)
include ( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri)
include ( $${PACKAGE_LIBS_SOURCES}/sdk_medintuxtoolkit.pri)
INCLUDEPATH += $${PWD}
QT *= sql \
    network \
    xml
DEFINES *= DRUGS_INTERACTIONS_STANDALONE
include ( $${PACKAGE_LIBS_SOURCES}/rpath.pri)

# symbol export/import for toolkit
DEFINES *= FMF_CORE_BUILD

# mac:*-g++:LIBS *= -Wl,-all_load # import all symbols as the not used ones too
# else:*-g++:LIBS *= -Wl,--whole-archive # import all symbols as the not used ones too
# mac:*-g++:LIBS *= -dynamic
# else:unix:*-g++:LIBS *= -rdynamic
# informs pretargets dependencies
PRE_TARGETDEPS *= $${PACKAGE_LIBS_SOURCES}/toolkit \
    $${PACKAGE_LIBS_SOURCES}/medintuxtoolkit
LIBS *= -L$${PACKAGE_LIBS_BIN}
mac:*-g++:LIBS *= -Wl,-noall_load # stop importing all symbols
else:*-g++:LIBS *= -Wl,--no-whole-archive # stop importing all symbols

# include mfDrugsWidgets sources/headers/ui/translations && resources
include( ../../plugins/formwidgets/mfDrugsWidget/mfdrugswidget.pri )

# sources
SOURCES += main.cpp \
    IamTester.cpp
HEADERS += IamTester.h
