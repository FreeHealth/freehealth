TEMPLATE         = app
TARGET           = FreeDiams
mac:TARGET       = $$quote(FreeDiams)
PACKAGE_VERSION  = 0.0.9

# include general configuration
INSTALL_DRUGS = 1
include( ../config.pri )
!CONFIG(debug, release|debug):include( ../buildspecs/install.pri )

# include SDKs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

# include Doc
# !CONFIG(crosscompil) {
include( ../doc/manual-di.pri)
# PRE_TARGETDEPS += html_docs
# }

# define CORE BUILD for fmf exporter
DEFINES *= CORE_BUILD

QT *= sql \
    network \
    xml
DEFINES *= FREEDIAMS

# sources
SOURCES += \
#    diMainWindow.cpp \
    main.cpp \
#    diCore.cpp \
#    diAboutDialog.cpp \
#    diPatient.cpp

# headers
#HEADERS += \
#    diMainWindow.h \
#    diCore.h \
#    diAboutDialog.h \
#    diMedinTux.h \
#    diPatient.h

# check protected sources
#exists( $${PROTECTED_PATH} ):SOURCES += $${PROTECTED_PATH}/freediams/diMedinTux_Pro.cpp
#else:SOURCES += diMedinTux.cpp
#FORMS += diMainWindow.ui
