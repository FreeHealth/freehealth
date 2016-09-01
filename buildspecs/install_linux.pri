!CONFIG(debug_without_install) {
 !linux*:!hurd*:!glibc*:error(install_linux.pri should only be used when building linux configuration)
 !CONFIG(LINUX_INTEGRATED):message(Preparing installation for Unix)
 else:message(Preparing installation for integrated Linux)

 QMAKE_INSTALL_FILE=cp -a
 QMAKE_INSTALL_PROGRAM=cp -a

 CONFIG(LINUX_INTEGRATED){
   # Redefine the default install paths
   isEmpty(INSTALL_ROOT_PATH):INSTALL_ROOT_PATH = $${SOURCES_ROOT_PATH}/packages/linux-integrated/usr
   INSTALL_BINARY_PATH    = $${INSTALL_ROOT_PATH}/bin
   INSTALL_LIBS_PATH      = $${INSTALL_ROOT_PATH}/$${LIBRARY_BASENAME}/freehealth-common
   INSTALL_PLUGINS_PATH   = $${INSTALL_ROOT_PATH}/$${LIBRARY_BASENAME}/$${LOWERED_APPNAME}
   INSTALL_RESOURCES_PATH = $${INSTALL_ROOT_PATH}/share/$${LOWERED_APPNAME}
   INSTALL_DOCS_PATH      = $${INSTALL_ROOT_PATH}/share/doc/freehealth/$${LOWERED_APPNAME}

   isEmpty(INSTALL_DESKTOP_FILES_PATH):INSTALL_DESKTOP_FILES_PATH = $${INSTALL_ROOT_PATH}/share/applications/
   isEmpty(INSTALL_DESKTOP_ICON_PATH):INSTALL_DESKTOP_ICON_PATH = $${INSTALL_ROOT_PATH}/share/pixmaps

   # Do not install Qt libs inside the bundle (leave empty)
   INSTALL_QT_INSIDE_BUNDLE  =
   INSTALL_QT_LIBS_PATH      =
   INSTALL_QT_PLUGINS_PATH   =
   INSTALL_BINARY_WRAPPER_NAME =

   # create a link in /usr/bin to the application
   !isEmpty(LINK_ROOT_PATH){
     createlink.commands = ln -sf $${INSTALL_BINARY_PATH}/$${BINARY_TARGET} $${LINK_ROOT_PATH}/$${BINARY_TARGET}
     QMAKE_EXTRA_TARGETS += createlink
     PRE_TARGETDEPS += createlink
   }
 }else{
    # define root path and install path
    isEmpty(INSTALL_ROOT_PATH):INSTALL_ROOT_PATH = $${RELEASE_BINARY_PATH}/$${INSTALL_BASENAME_PATH}/$${BINARY_TARGET}
    INSTALL_BINARY_PATH    = $${INSTALL_ROOT_PATH}
    INSTALL_LIBS_PATH      = $${INSTALL_BINARY_PATH}/plugins
    INSTALL_PLUGINS_PATH   = $${INSTALL_LIBS_PATH}

    # install binary wrapper with rpath redefinition
    INSTALL_BINARY_WRAPPER_NAME = $${TARGET}-linux

    # install Qt libs and plugs
    INSTALL_QT_INSIDE_BUNDLE = 1
    INSTALL_QT_LIBS_PATH     = $${INSTALL_BINARY_PATH}/plugins
    INSTALL_QT_PLUGINS_PATH  = $${INSTALL_BINARY_PATH}/plugins/qt

    message( Linux Bundle : Qt Libs will be installed from $$[QT_INSTALL_LIBS] to $${INSTALL_QT_LIBS_PATH})
    message( Linux Bundle : Binary Wrapper will be : $${INSTALL_BINARY_WRAPPER_NAME})
 }

   LIB_EXTENSION = so*

}  #end if release mode


