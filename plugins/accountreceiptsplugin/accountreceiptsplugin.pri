include(accountreceiptsplugin_dependencies.pri)
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAccountReceipts_debug -lAccountBase_debug
    win32:LIBS   *= -lAccountReceipts_d
} else {
    LIBS  *= -lAccountReceipts
}
