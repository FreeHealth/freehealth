include(shared_sources.pri)

!with-edrc{
    error(eDRC plugin not requested)
} else {
    message(Building eDRC plugin)
}

greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= printsupport
}

# include EHR specific sources
HEADERS += \
    ehr/edrcwidgetfactory.h

SOURCES += \
    ehr/edrcwidgetfactory.cpp

OTHER_FILES += eDRC.pluginspec
