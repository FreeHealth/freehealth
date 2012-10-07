TEMPLATE = lib
TARGET = InteractionDB

include(../../../plugins/fmf_plugins.pri)
include(interactiondb_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += interactionplugin.h \
    interactionstep.h \
    drugdruginteraction.h \
    drugdruginteractionmodel.h \
    drugdruginteractioncore.h \
    interactioneditorpage.h \
    formalizeditemmodel.h \
    druginteractor.h \
    interactoreditorpage.h \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.h \
    iamdbcore.h

#    cytochromep450interactionspage.h \

SOURCES += interactionplugin.cpp \
    interactionstep.cpp \
    drugdruginteraction.cpp \
    drugdruginteractionmodel.cpp \
    drugdruginteractioncore.cpp \
    interactioneditorpage.cpp \
    formalizeditemmodel.cpp \
    druginteractor.cpp \
    interactoreditorpage.cpp \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \
    iamdbcore.cpp
#    cytochromep450interactionspage.cpp \

SOURCES += \
    afssapsintegrator.cpp \
    interactionsdatabasepage.cpp


HEADERS  += \
    afssapsintegrator.h \
    interactionsdatabasepage.h


FORMS    += afssapslinkerwidget.ui \
    afssapstreewidget.ui \
    interactioneditorwidget.ui \
    interactiondatabasecreator.ui \
    interactiondatabasechecker.ui \
    afssapsintegratorwidget.ui \
    interactoreditorwidget.ui
#    cytochromep450interactionspage.ui \

OTHER_FILES += \
    ../global_resources/sql/drugs_schema.sql \
    ../global_resources/sql/iam_schema.sql \
    ../global_resources/sql/ia/atc_utf8.csv \
    ../global_resources/sql/ia/thesaurus_interactions.xml \
    ../global_resources/sql/ia/afssaps_links.xml \
    ../global_resources/sql/ia/afssaps_classtree.xml \
    ../global_resources/sql/drugdb/newdbschema.sql \
    ../global_resources/sql/drugdb/dbschema060_queries.sql \


OTHER_FILES += InteractionDB.pluginspec















