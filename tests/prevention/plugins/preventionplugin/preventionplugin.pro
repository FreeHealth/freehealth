TEMPLATE = lib
TARGET = Prevention

DEFINES += PREVENTION
#PREVENTION = 1

BUILD_PATH_POSTFIXE = Prevention

include(../../../../plugins/fmf_plugins.pri)
include(preventionplugin_dependencies.pri)
include(preferences/prevention_preferences.pri)

QT += sql

INCLUDEPATH += ../
DEPENDPATH += ../

# Input
HEADERS += preventionplugin.h\
           preventCore.h \
           preventIHM.h \
           preventIO.h \
           treeviewdelegate.h \
           moreIHM.h \
           survey.h \
           nextdateitemsdialog.h \
           #preventionconstants.h \
           IPrevention.h \
           connexion.h

SOURCES += preventionplugin.cpp \
           preventCore.cpp \
           preventIHM.cpp \
           preventIO.cpp \
           treeviewdelegate.cpp \
           moreIHM.cpp \
           survey.cpp \
           nextdateitemsdialog.cpp \
           connexion.cpp
FORMS += prevent.ui \
         nextdateitemsdialog.ui \   
         moreIHM.ui

OTHER_FILES += Core.pluginspec

