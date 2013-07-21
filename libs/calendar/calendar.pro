TEMPLATE = lib
TARGET = Calendar

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

DEFINES *= CALENDAR_LIBRARY

# include config file
include(../libsworkbench.pri)
include(../calendar_dependencies.pri)

HEADERS += calendar_exporter.h \
    calendar_widget.h \
    common.h \
    view.h \
    icalendaritemdatawidget.h \
    calendar_theme.h \
    calendar_navbar.h \
    modelanditem/abstract_calendar_model.h \
    modelanditem/basic_calendar_model.h \
    modelanditem/basic_item_edition_dialog.h \
    modelanditem/calendar_item.h \
    modelanditem/calendar_item_widget.h \
    modelanditem/item_editor_widget.h \
    modelanditem/calendar_people.h \
    dayview/dayrangeheader.h \
    dayview/dayrangebody.h \
    dayview/hour_range_widget.h \
    dayview/hour_range_node.h \
    dayview/hourwidget.h \
    dayview/day_widget.h \
    dayview/day_node.h \
    monthview/month_day_widget.h \
    monthview/month_view.h \
    hourmark.h

SOURCES += \
    calendar_navbar.cpp \
    calendar_widget.cpp \
    common.cpp \
    view.cpp \
    icalendaritemdatawidget.cpp \
    calendar_theme.cpp \
    modelanditem/abstract_calendar_model.cpp \
    modelanditem/basic_calendar_model.cpp \
    modelanditem/basic_item_edition_dialog.cpp \
    modelanditem/calendar_item.cpp \
    modelanditem/calendar_item_widget.cpp \
    modelanditem/calendar_people.cpp \
    modelanditem/item_editor_widget.cpp \
    dayview/dayrangeheader.cpp \
    dayview/dayrangebody.cpp \
    dayview/hour_range_widget.cpp \
    dayview/hour_range_node.cpp \
    dayview/hourwidget.cpp \
    dayview/day_widget.cpp \
    dayview/day_node.cpp \
    monthview/month_day_widget.cpp \
    monthview/month_view.cpp \
    hourmark.cpp

FORMS += modelanditem/basic_item_edition_dialog.ui \
    modelanditem/item_editor_widget.ui

# include translations
TRANSLATION_NAME = calendar
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
