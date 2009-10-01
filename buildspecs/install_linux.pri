CONFIG( debug, debug|release ) {
message( No installation available in Debug mode )
}
else {
 !linux*:error(install_linux.pri should only be used when building linux configuration)
 !CONFIG(LINUX_INTEGRATED):message(Preparing installation for Unix)
 else:message(Preparing installation for integrated Linux)

 CONFIG(LINUX_INTEGRATED){
   # Redefine the default install paths
   isEmpty(INSTALL_ROOT_PATH):INSTALL_ROOT_PATH = $${SOURCES_ROOT_PATH}/packages/linux-integrated/usr
   INSTALL_BINARY_PATH    = $${INSTALL_ROOT_PATH}/bin
   INSTALL_LIBS_PATH      = $${INSTALL_ROOT_PATH}/$${LIBRARY_BASENAME}/$${TARGET}
   INSTALL_PLUGINS_PATH   = $${INSTALL_LIBS_PATH}
   INSTALL_RESOURCES_PATH = $${INSTALL_ROOT_PATH}/share/$${TARGET}
   isEmpty(INSTALL_DESKTOP_FILES_PATH):INSTALL_DESKTOP_FILES_PATH = $${INSTALL_ROOT_PATH}/share/applications/
   isEmpty(INSTALL_DESKTOP_ICON_PATH):INSTALL_DESKTOP_ICON_PATH = $${INSTALL_ROOT_PATH}/share/pixmaps

   # Do not install Qt libs inside the bundle (leave empty)
   INSTALL_QT_INSIDE_BUNDLE  =
   INSTALL_QT_LIBS_PATH      =
   INSTALL_QT_PLUGINS_PATH   =

   # create a link in /usr/bin to the application
   !isEmpty(LINK_ROOT_PATH){
     createlink.commands = ln -sf $${INSTALL_BINARY_PATH}/$${BINARY_TARGET} $${LINK_ROOT_PATH}/$${BINARY_TARGET}
     QMAKE_EXTRA_TARGETS += createlink
     PRE_TARGETDEPS += createlink
   }
 }else{
   # install binary wrapper with rpath redefinition
 }

   LIB_EXTENSION = so*

}  #end if release mode


