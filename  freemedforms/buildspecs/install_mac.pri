CONFIG( debug, debug|release ) {
message( No installation available in Debug mode )
}
else {
 !macx:error(install_mac.pri should only be used when building Mac version)
 # Qt libs and plugs are installed by the bash scripts
 INSTALL_QT_LIBS_PATH    =
 INSTALL_QT_PLUGINS_PATH =

 INSTALL_ROOT_PATH       = $${RELEASE_BINARY_PATH}/$${INSTALL_BASENAME_PATH}/$${BINARY_TARGET}
 INSTALL_BINARY_PATH     = $${INSTALL_ROOT_PATH}
 INSTALL_LIBS_PATH       = $${INSTALL_BINARY_PATH}/$${BINARY_TARGET}.app/Contents/plugins
 INSTALL_PLUGINS_PATH    = $${INSTALL_LIBS_PATH}
 INSTALL_RESOURCES_PATH  = $${INSTALL_BINARY_PATH}/$${BINARY_TARGET}.app/Contents/Resources

 LIB_EXTENSION           = $${QMAKE_EXTENSION_SHLIB}

}  #end if release mode


