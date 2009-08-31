CONFIG( debug, debug|release ) {
message( No installation available in Debug mode )
}
else {
 !linux*:error(install_linux.pri should only be used when building linux configuration)
 !CONFIG(LINUX_INTEGRATED):message(Preparing installation for Unix)
 else:message(Preparing installation for integrated Linux)

 CONFIG(LINUX_INTEGRATED){
   INSTALL_QT_INSIDE_BUNDLE  =
   # Qt libs and plugs
   INSTALL_QT_LIBS_PATH      =
   INSTALL_QT_PLUGINS_PATH   =

   # create a link in /usr/bin to the application
   !isEmpty(LINK_ROOT_PATH){
     createlink.commands = ln -sf $${INSTALL_ROOT_PATH}/$${BINARY_TARGET} $${LINK_ROOT_PATH}/$${BINARY_TARGET}
     QMAKE_EXTRA_TARGETS += createlink
     PRE_TARGETDEPS += createlink
   }
 }else{
   # install binary wrapper with rpath redefinition
 }

   LIB_EXTENSION = so*

}  #end if release mode


