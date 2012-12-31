#-------------------------------------------------
#
# Project created by QtCreator 2012-12-31T00:46:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = frenchnsstester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        ../../plugins/basewidgetsplugin/frenchsocialnumberwidget.cpp

HEADERS  += mainwindow.h \
        ../../plugins/basewidgetsplugin/frenchsocialnumberwidget.h

FORMS    += mainwindow.ui \
    ../../plugins/basewidgetsplugin/frenchsocialnumber.ui
