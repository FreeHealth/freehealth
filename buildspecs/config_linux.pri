linux-g++{

  !CONFIG(debug, debug|release):CONFIG(LINUX_INTEGRATED){
     message( *********** Building Linux Integrated Spec ********** )
     DEFINES *= LINUX_INTEGRATED
     DEFINES *= "LINUX_QT_PATH=\"\\\"$$[QT_INSTALL_LIBS]\\\"\"" \
                "LINUX_QT_PLUGINS_PATH=\"\\\"$$[QT_INSTALL_PLUGINS]\\\"\""
  }

  RPATH_LIBS_BIN     = plugins/

  CONFIG( debug, debug|release ) {
    CONFIG      += console
  }

}
