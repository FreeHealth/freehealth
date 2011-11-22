#-------------------------------------------------
#
# Project created by QtCreator 2011-11-19T22:16:23
#
#-------------------------------------------------

QT       += core gui sql xml

TARGET = nextavail
TEMPLATE = app

include( ../../config.pri )

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

# add the next availability calculator
HEADERS += $${SOURCES_PLUGINS_PATH}/agendaplugin/nextavailabiliystepviewer.h
SOURCES += $${SOURCES_PLUGINS_PATH}/agendaplugin/nextavailabiliystepviewer.cpp
FORMS += $${SOURCES_PLUGINS_PATH}/agendaplugin/nextavailabiliystepviewer.ui

# add some lib / utils
HEADERS += $${SOURCES_LIBS_PATH}/utils/global.h
SOURCES += $${SOURCES_LIBS_PATH}/utils/global.cpp
HEADERS += $${SOURCES_LIBS_PATH}/utils/log.h
SOURCES += $${SOURCES_LIBS_PATH}/utils/log.cpp
HEADERS += $${SOURCES_LIBS_PATH}/utils/licenseterms.h
SOURCES += $${SOURCES_LIBS_PATH}/utils/licenseterms.cpp
HEADERS += $${SOURCES_LIBS_PATH}/translationutils/constanttranslations.h
SOURCES += $${SOURCES_LIBS_PATH}/translationutils/constanttranslations.cpp
