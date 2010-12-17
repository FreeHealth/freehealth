TARGET = drugsbasetester
CONFIG += console
TEMPLATE = app

CONFIG -= app_bundle

# include SDKs
INCLUDEPATH += $${PWD}
QT *= sql

# sources
SOURCES += main.cpp
