include(drugsbaseplugin_dependencies.pri)
CONFIG( debug, debug|release ) { 
    unix:LIBS *= -lDrugsBase_debug
    win32:LIBS *= -lDrugsBase_d
}
else:LIBS *= -lDrugsBase
