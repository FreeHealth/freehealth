TEMPLATE = lib
TARGET = InteractionDB
PACKAGE_VERSION = 0.0.2

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include(interactiondb_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += interactionplugin.h \
    interactionstep.h
#    cytochromep450interactionspage.h \

SOURCES += interactionplugin.cpp \
    interactionstep.cpp
#    cytochromep450interactionspage.cpp \

SOURCES += \
    afssapsintegrator.cpp \
    interactionsdatabasepage.cpp


HEADERS  += \
    afssapsintegrator.h \
    interactionsdatabasepage.h


FORMS    += afssapslinkerwidget.ui \
    afssapstreewidget.ui \
    interactiondatabasebuilder.ui \
    interactiondatabasecreator.ui \
    interactiondatabasechecker.ui \
    afssapsintegratorwidget.ui
#    cytochromep450interactionspage.ui \

OTHER_FILES += \
    ../global_resources/sql/drugs_schema.sql \
    ../global_resources/sql/iam_schema.sql \
    ../global_resources/sql/atc_utf8.csv \
    ../global_resources/sql/thesaurus_interactions.xml \
    ../global_resources/sql/afssaps_links.xml \
    ../global_resources/sql/afssaps_classtree.xml \

OTHER_FILES += InteractionDB.pluginspec
