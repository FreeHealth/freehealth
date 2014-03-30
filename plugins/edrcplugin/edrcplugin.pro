include(shared_sources.pri)

!with-edrc{
    error(eDRC plugin not requested)
} else {
    message(Building eDRC plugin)
}

greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= printsupport
}

# include FreeMedForms specific sources
HEADERS += \
    freemedforms/edrcwidgetfactory.h

SOURCES += \
    freemedforms/edrcwidgetfactory.cpp

OTHER_FILES += eDRC.pluginspec
