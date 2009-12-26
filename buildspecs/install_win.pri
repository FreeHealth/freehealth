CONFIG( debug, debug|release ) {
  message( No installation available in Debug mode )
} else {
 !win32:error(install_win.pri should only be used when building win32 configuration)
 LIB_EXTENSION = dll

 CONFIG(crosscompil) {
    QTLIBS_INSTALL_PATH = $${TARGET_INSTALL_PATH}
    QTLIBS_PATH = /home/eric/.wine/drive_c/Qt/2009.02/qt/bin
    QTPLUGINS_PATH = $${QTLIBS_PATH}/../plugins
    qtlibs.path  = $${QTLIBS_INSTALL_PATH}
    qtlibs.files = $${QTLIBS_PATH}/*mingw* \
                   $${QTLIBS_PATH}/*QtCore4* \
                   $${QTLIBS_PATH}/*QtGui4* \
                   $${QTLIBS_PATH}/*QtSql4* \
                   $${QTLIBS_PATH}/*QtScript4* \
                   $${QTLIBS_PATH}/*Qt3Support4* \
                   $${QTLIBS_PATH}/*QtXml4* \
                   $${QTLIBS_PATH}/*QtSvg4* \
                   $${QTLIBS_PATH}/*QtNetwork4*
    INSTALLS += qtlibs
    message( Cross-compilation : Qt Libs will be installed from $${QTLIBS_PATH} to $${QTLIBS_INSTALL_PATH})
 }else{
    QTLIBS_INSTALL_PATH = $${TARGET_INSTALL_PATH}
    QTLIBS_PATH = $$[QT_INSTALL_LIBS]
    QTPLUGINS_PATH = $$[QT_INSTALL_PLUGINS]
    qtlibs.path  = $${QTLIBS_INSTALL_PATH}
    qtlibs.files = $$[QT_INSTALL_LIBS]/*QtCore* \
                   $$[QT_INSTALL_LIBS]/*QtGui* \
                   $$[QT_INSTALL_LIBS]/*QtSql* \
                   $$[QT_INSTALL_LIBS]/*QtScript* \
                   $$[QT_INSTALL_LIBS]/*Qt3Support* \
                   $$[QT_INSTALL_LIBS]/*QtXml* \
                   $$[QT_INSTALL_LIBS]/*QtSvg* \
                   $$[QT_INSTALL_LIBS]/*QtNetwork*
    INSTALLS += qtlibs
    message( Win32 : Qt Libs will be installed from $$[QT_INSTALL_LIBS] to $${QTLIBS_INSTALL_PATH})
 }

}  #end if release mode


