TARGET = mf_drugswidgetplugin

include( ../formwidgets.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_usertoolkit.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_medicaltoolkit.pri )

DESTDIR = $$FMF_PLUGINS_DIR

CONFIG *= sql

QT *= sql network

INCLUDEPATH += .

include( mfdrugswidget.pri)

HEADERS += mfDrugsWidget.h
SOURCES += mfDrugsWidget.cpp
