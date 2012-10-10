TEMPLATE = app
macx:win32:TARGET = $$quote(RapidComptaPorting)
else:TARGET = $$quote(rapidcomptaporting)

include(../../../buildspecs/config.pri)

QT += xml network core gui

# include libs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/translationutils.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

SOURCES = main.cpp

