# toolkit library project file
TEMPLATE = lib
TARGET = Utils
PACKAGE_VERSION = 0.4.0
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
    widgets/calendar/abstract_calendar_model.h \
    widgets/calendar/basic_calendar_model.h \
    widgets/calendar/calendar_item.h \
    widgets/calendar/calendar_item_widget.h \
    widgets/calendar/calendar_navbar.h \
    widgets/calendar/calendar_widget.h \
    widgets/calendar/common.h \
    widgets/calendar/day_range_view.h \
    widgets/calendar/month_view.h \
    widgets/calendar/view.h \
    widgets/calendar/day_item_widget.h \
    widgets/languagecombobox.h \
    widgets/languagecomboboxdelegate.h
#    widgets/dialogbuttonbox.h

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
    widgets/calendar/abstract_calendar_model.cpp \
    widgets/calendar/basic_calendar_model.cpp \
    widgets/calendar/calendar_item.cpp \
    widgets/calendar/calendar_item_widget.cpp \
    widgets/calendar/calendar_navbar.cpp \
    widgets/calendar/calendar_widget.cpp \
    widgets/calendar/common.cpp \
    widgets/calendar/day_range_view.cpp \
    widgets/calendar/month_view.cpp \
    widgets/calendar/view.cpp \
    widgets/calendar/day_item_widget.cpp \
    widgets/languagecombobox.cpp \
    widgets/languagecomboboxdelegate.cpp
#    widgets/dialogbuttonbox.cpp


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
