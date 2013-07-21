QT       += core gui sql

TARGET = zipcompleter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    zipcountrycompleters.cpp \
    frenchsocialnumber.cpp

HEADERS  += mainwindow.h \
    zipcountrycompleters.h \
    frenchsocialnumber.h

FORMS    += mainwindow.ui \
    frenchsocialnumber.ui
