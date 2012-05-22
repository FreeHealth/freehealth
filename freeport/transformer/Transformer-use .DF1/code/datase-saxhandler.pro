QT += sql
QT += sql

sql-plugins += odbc

TEMPLATE      = app
QT           += xml
CONFIG       += console
HEADERS       = saxhandler.h \
    patient.h \
    xmlwriter.h \
    xmlwriterblock.h \
    window.h
SOURCES       = main.cpp \
                saxhandler.cpp \
    patient.cpp \
    xmlwriter.cpp \
    xmlwriterblock.cpp \
    window.cpp
