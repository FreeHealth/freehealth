include(druginfos_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lDrugInfos_debug
    win32:LIBS   *= -lDrugInfos_d
} else {
    LIBS  *= -lDrugInfos
}
