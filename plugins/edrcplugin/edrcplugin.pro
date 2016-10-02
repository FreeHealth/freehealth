include(shared_sources.pri)

!with-edrc{
    error(eDRC plugin not requested)
} else {
    message(Building eDRC plugin)
}

QT *= printsupport

# include EHR specific sources
HEADERS += \
    ehr/edrcwidgetfactory.h

SOURCES += \
    ehr/edrcwidgetfactory.cpp

OTHER_FILES += eDRC.pluginspec
