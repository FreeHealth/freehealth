TEMPLATE = lib
TARGET = Calendar

DEFINES *= CALENDAR_LIBRARY

# include config file
include(../libsworkbench.pri)
include(../calendar_dependencies.pri)

HEADERS += calendar_exporter.h \
    abstract_calendar_model.h \
    basic_calendar_model.h \
    calendar_item.h \
    calendar_item_widget.h \
    calendar_navbar.h \
    calendar_widget.h \
    common.h \
    day_range_view.h \
    month_view.h \
    view.h \
    hour_range_widget.h \
    hour_range_node.h \
    month_day_widget.h \
    basic_item_edition_dialog.h \
    day_widget.h \
    day_node.h \
    usercalendar.h \

SOURCES += \
    abstract_calendar_model.cpp \
    basic_calendar_model.cpp \
    calendar_item.cpp \
    calendar_item_widget.cpp \
    calendar_navbar.cpp \
    calendar_widget.cpp \
    common.cpp \
    day_range_view.cpp \
    month_view.cpp \
    view.cpp \
    hour_range_widget.cpp \
    hour_range_node.cpp \
    month_day_widget.cpp \
    basic_item_edition_dialog.cpp \
    day_widget.cpp \
    day_node.cpp \
    usercalendar.cpp \

FORMS += basic_item_edition_dialog.ui

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/calendar_fr.ts \
    $${SOURCES_TRANSLATIONS_PATH}/calendar_de.ts \
    $${SOURCES_TRANSLATIONS_PATH}/calendar_es.ts


