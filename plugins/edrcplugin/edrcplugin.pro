include(shared_sources.pri)

!with-edrc{
    error(eDRC plugin not requested)
} else {
    message(Building eDRC plugin)
}

# include FreeMedForms specific sources
HEADERS += \
    freemedforms/edrcwidgetfactory.h

SOURCES += \
    freemedforms/edrcwidgetfactory.cpp

OTHER_FILES += eDRC.pluginspec
