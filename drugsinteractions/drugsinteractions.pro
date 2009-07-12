TEMPLATE         = app
TARGET           = drugsinteractions
mac:TARGET       = $$quote(drugsinteractions)
PACKAGE_VERSION  = 0.0.9

# include general configuration
include( ../config.pri )

# include SDKs
include ( ../plugins/sdk_drugswidget.pri)
include ( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri)
include ( $${PACKAGE_LIBS_SOURCES}/sdk_medintuxtoolkit.pri)
INCLUDEPATH += $${PWD}

# prepare installation
INSTALL_DRUGS = 1
include ( ../install.pri)
QT *= sql \
    network \
    xml
DEFINES *= DRUGS_INTERACTIONS_STANDALONE
include ( $${PACKAGE_LIBS_SOURCES}/rpath.pri)

# symbol export/import for toolkit
DEFINES *= FMF_CORE_BUILD

LIBS *= -L$${PACKAGE_LIBS_BIN}

# prepare documentation, only when release, swap crosscompil
#!CONFIG( debug, debug|release ) {
  !CONFIG(crosscompil) {
    include( ../doc/di-manual.pri)
    PRE_TARGETDEPS += html_docs
  }
#}

mac:*-g++:LIBS *= -Wl,-noall_load # stop importing all symbols
else:*-g++:LIBS *= -Wl,--no-whole-archive # stop importing all symbols

# include mfDrugsWidgets sources/headers/ui/translations && resources
include( ../plugins/formwidgets/mfDrugsWidget/mfdrugswidget.pri )

# sources
SOURCES += diMainWindow.cpp \
    main.cpp \
    diCore.cpp \
    diAboutDialog.cpp

# headers
HEADERS += diMainWindow.h \
    diCore.h \
    diAboutDialog.h \
    diMedinTux.h

# check protected sources
exists( $${PROTECTED_PATH} ):SOURCES += $${PROTECTED_PATH}/drugsinteractions/diMedinTux_Pro.cpp
else:SOURCES += diMedinTux.cpp
FORMS += diMainWindow.ui
