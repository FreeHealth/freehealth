TEMPLATE = lib
TARGET = Utils

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

DEFINES *= UTILS_LIBRARY

# include config file
include(../libsworkbench.pri)
include(../utils_dependencies.pri)

HEADERS += global_exporter.h \
    global.h \
    database.h \
    log.h \
    updatechecker_p.h \
    updatechecker.h \
    messagesender.h \
    serializer.h \
    licenseterms.h \
    randomizer.h \
    stylehelper.h \
    httpdownloader.h \
    iconbadgealert.h \
    qtcassert.h \
    widgets/scrollingwidget.h \
    widgets/lineeditechoswitcher.h \
    widgets/qbuttonlineedit.h \
    widgets/QPixLineEdit.h \
    widgets/colorbuttonchooser.h \
    widgets/spinboxdelegate.h \
    widgets/styledbar.h \
    widgets/fancyactionbar.h \
    widgets/fancytabwidget.h \
    widgets/minisplitter.h \
    widgets/combowithfancybutton.h \
    widgets/basevalidatinglineedit.h \
    widgets/filenamevalidatinglineedit.h \
    widgets/pathchooser.h \
    pubmeddownloader.h \
    qabstractxmltreemodel.h \
    widgets/datetimedelegate.h \
    widgets/languagecombobox.h \
    widgets/languagecomboboxdelegate.h \
    databaseconnector.h \
    versionnumber.h \
    widgets/countrycombobox.h \
    idatabasedumper.h \
    widgets/imageviewer.h \
    genericupdateinformation.h \
    genericdescription.h \
    widgets/genericdescriptioneditor.h \
    widgets/genericupdateinformationeditor.h \
    widgets/genericinformationeditordialog.h \
    widgets/htmldelegate.h \
    widgets/uppercasevalidator.h \
    widgets/loginwidget.h \
    widgets/basiclogindialog.h \
    widgets/fontselectorbutton.h \
    widgets/fontandcolorsselectorwidget.h \
    widgets/segmentedbutton.h \
    widgets/periodselectortoolbutton.h \
    widgets/birthdayedit.h \
    datevalidator.h \
    widgets/fadewidgethack.h \
    widgets/detailswidget.h \
    widgets/detailsbutton.h \
    proxyaction.h \
    stringutils.h \
    widgets/timecombobox.h

SOURCES += global.cpp \
    database.cpp \
    log.cpp \
    updatechecker.cpp \
    messagesender.cpp \
    licenseterms.cpp \
    serializer.cpp \
    randomizer.cpp \
    stylehelper.cpp \
    httpdownloader.cpp \
    widgets/scrollingwidget.cpp \
    widgets/lineeditechoswitcher.cpp \
    widgets/qbuttonlineedit.cpp \
    widgets/colorbuttonchooser.cpp \
    widgets/spinboxdelegate.cpp \
    widgets/styledbar.cpp \
    widgets/fancyactionbar.cpp \
    widgets/fancytabwidget.cpp \
    widgets/minisplitter.cpp \
    widgets/combowithfancybutton.cpp \
    widgets/basevalidatinglineedit.cpp \
    widgets/filenamevalidatinglineedit.cpp \
    widgets/pathchooser.cpp \
    pubmeddownloader.cpp \
    qabstractxmltreemodel.cpp \
    widgets/datetimedelegate.cpp \
    widgets/languagecombobox.cpp \
    widgets/languagecomboboxdelegate.cpp \
    databaseconnector.cpp \
    versionnumber.cpp \
    widgets/countrycombobox.cpp \
    idatabasedumper.cpp \
    widgets/imageviewer.cpp \
    genericupdateinformation.cpp \
    genericdescription.cpp \
    widgets/genericdescriptioneditor.cpp \
    widgets/genericupdateinformationeditor.cpp \
    widgets/genericinformationeditordialog.cpp \
    widgets/htmldelegate.cpp \
    widgets/loginwidget.cpp \
    widgets/basiclogindialog.cpp \
    widgets/uppercasevalidator.cpp \
    widgets/fontselectorbutton.cpp \
    widgets/fontandcolorsselectorwidget.cpp \
    widgets/segmentedbutton.cpp \
    widgets/periodselectortoolbutton.cpp \
    widgets/birthdayedit.cpp \
    datevalidator.cpp \
    widgets/fadewidgethack.cpp \
    widgets/detailswidget.cpp \
    widgets/detailsbutton.cpp \
    proxyaction.cpp \
    stringutils.cpp \
    widgets/timecombobox.cpp

macx {
    OBJECTIVE_SOURCES += iconbadgealert.mm
    LIBS += -framework Carbon -framework AppKit
} else {
    SOURCES += iconbadgealert.cpp
}

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/utils_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/utils_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/utils_es.ts

# resources
#RESOURCES += $${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/fancytoolbutton.svg

FORMS += \
    widgets/genericdescriptioneditor.ui \
    widgets/genericupdateinformationeditor.ui \
    widgets/genericinformationeditordialog.ui \
    widgets/loginwidget.ui \
    widgets/basiclogindialog.ui
