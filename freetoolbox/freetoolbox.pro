#-------------------------------------------------
#
# Project created by QtCreator 2010-07-31T11:29:45
#
#-------------------------------------------------

QT       += core gui

TARGET = freetoolbox
TEMPLATE = app
#macx {
# CONFIG -= app_bundle
#}


include(../config.pri)
include($${SOURCES_LIBS_PATH}/utils.pri)
include(../contrib/quazip/quazip.pri)
include($${SOURCES_LIBS_PATH}/rpath.pri)

LIBS *= -L$${SOURCES_ROOT_PATH}/bin/

DEFINES *= FREETOOLBOX

SOURCES += main.cpp\
        mainwindow.cpp \
    canadiandrugsdatabase.cpp \
    canadiandrugsdatabasewidget.cpp \
    globaltools.cpp

HEADERS  += mainwindow.h \
    itoolpage.h \
    constants.h \
    canadiandrugsdatabase.h \
    canadiandrugsdatabasewidget.h \
    globaltools.h

FORMS    += mainwindow.ui \
    canadiandrugsdatabasewidget.ui
