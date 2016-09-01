linux*{

  !CONFIG(debug_without_install):CONFIG(LINUX_INTEGRATED){
     #message( *********** Building Linux Integrated Spec ********** )
     DEFINES *= LINUX_INTEGRATED
     DEFINES *= "LINUX_QT_PATH=\"\\\"$$[QT_INSTALL_LIBS]\\\"\"" \
                "LINUX_QT_PLUGINS_PATH=\"\\\"$$[QT_INSTALL_PLUGINS]\\\"\""

     # the library base name is used only during the install procedure to locate the path
    # the application's libraries and plugins should be installed
    # you can specify it when compiling = qmake -r -config release "LIBRARY_BASENAME=lib64" for 64bit compilation
    # this value is only used with linux compilation
    isEmpty(LIBRARY_BASENAME):LIBRARY_BASENAME = lib
    # this part must be synchronized with the install-linux.pri
    isEmpty(INSTALL_ROOT_PATH):INSTALL_ROOT_PATH = $${SOURCES_ROOT_PATH}/packages/linux-integrated/usr
    RPATH_LIBS_BIN     = $${INSTALL_ROOT_PATH}/$${LIBRARY_BASENAME}/$${TARGET}

    DEFINES *= "LIBRARY_BASENAME=\"\\\"$$LIBRARY_BASENAME\\\"\"" \
               "LINUX_PLUGIN_PATH=\"\\\"$$RPATH_LIBS_BIN\\\"\""

  } else {
    RPATH_LIBS_BIN     = plugins/
  }

  RPATH_LIBS_BIN     = plugins/
}
