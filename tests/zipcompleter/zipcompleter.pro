QT       += core gui sql

TARGET = zipcompleter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    zipcountrycompleters.cpp

HEADERS  += mainwindow.h \
    zipcountrycompleters.h

FORMS    += mainwindow.ui
