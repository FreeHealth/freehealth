# Preparing installation of all the resources into the application bundle
# Bundle structure can be found into tkSettings documentation
# All common resources are located into --> FMF_GLOBAL_RESOURCES

# This is the structure
#       MacOSX                                Linux/Win32
#
#       ApplicationName.app                   ApplicationName            == FMF_RELEASE_PACKAGES_PATH / ApplicationName
#       |- Contents                           |
#          |- MacOs                           |
#          |   `- applicationbinary           |- applicationbinary
#          |                                  |
#          |- Resources                       |- Resources
#          |   |                              |  |
#          |   |- CONFIG.INI                  |  |- CONFIG.INI            <-- if user can write into bundle
#          |   |                              |  |
#          |   |- databases                   |  |- databases
#          |   |  |- datas                    |  |  |- datas              <-- if user can write into bundle
#          |   |  |- drugs                    |  |  |- drugs              <-- if user can write into bundle (for dosages)
#          |   |  `- users                    |  |  `- users              <-- if user can write into bundle
#          |   |                              |  |
#          |   |- doc/application/html        |  |- doc/application/html
#          |   |                              |  |
#          |   |- forms                       |  |- forms
#          |   |                              |  |
#          |   |- pixmap                      |  |- pixmap                <-- Default Theme
#          |   |  |- 16x16                    |  |  |- 16x16
#          |   |  `- 32x32                    |  |  `- 32x32
#          |   `- translations                |  `- translations
#          |                                  |
#          |- libs                            |- libs                    <-- toolkits
#          |                                  |
#          |- plugins                         |- plugins
#          |  |- qt                           |  |- qt
#          |                                  |
#          `- FrameWorks (Qt FrameWorks)     (`- libs (Qt if needed) )

CONFIG( debug, debug|release ) {
message( No installation available in Debug mode )
}
else {
 # Message
 message( ***************************************************** )
 message( *************** INSTALLATION PROCESS **************** )
 message( ***************************************************** )

 # Install binary
 CONFIG(crosscompil) {
    message(Preparing installation for CrossCompilation Win32)
    TARGET_INSTALL_PATH     = $${FMF_RELEASE_PACKAGES_PATH}/win/$${PACKAGE_TARGET}
    TARGET_BINARY           = $${TARGET_INSTALL_PATH}/$${PACKAGE_TARGET}
    PLUGINS_INSTALL_PATH    = $${TARGET_INSTALL_PATH}/plugins
    RESOURCES_INSTALL_PATH  = $${TARGET_INSTALL_PATH}/Resources
    LIBS_INSTALL_PATH       = $${TARGET_INSTALL_PATH}
    LIB_EXTENSION           = dll

    installtarget.path = $${TARGET_INSTALL_PATH}
    installtarget.files = $${PACKAGE_DESTDIR}/$${PACKAGE_TARGET}.exe
    INSTALLS += installtarget
 } else {
     macx{
        message(Preparing installation for Mac X)
        TARGET_INSTALL_PATH      = $${FMF_RELEASE_PACKAGES_PATH}/mac/$${PACKAGE_TARGET}
        TARGET_BINARY            = $${TARGET_INSTALL_PATH}/$${PACKAGE_TARGET}
        PLUGINS_INSTALL_PATH     = $${TARGET_INSTALL_PATH}/$${PACKAGE_TARGET}.app/Contents/plugins
        RESOURCES_INSTALL_PATH   = $${TARGET_INSTALL_PATH}/$${PACKAGE_TARGET}.app/Contents/Resources
        LIBS_INSTALL_PATH        = $${TARGET_INSTALL_PATH}/$${PACKAGE_TARGET}.app/Contents/libs
        LIB_EXTENSION            = $${QMAKE_EXTENSION_SHLIB}

        target.path = $${TARGET_INSTALL_PATH}
        #target.files = $${PACKAGE_DESTDIR}/$${PACKAGE_TARGET}
        INSTALLS += target
     } else:unix{
        message(Preparing installation for Unix)
        TARGET_INSTALL_PATH      = $${FMF_RELEASE_PACKAGES_PATH}/linux/$${PACKAGE_TARGET}
        TARGET_BINARY            = $${TARGET_INSTALL_PATH}/$${PACKAGE_TARGET}.bin
        PLUGINS_INSTALL_PATH     = $${TARGET_INSTALL_PATH}/plugins
        RESOURCES_INSTALL_PATH   = $${TARGET_INSTALL_PATH}/Resources
        LIBS_INSTALL_PATH        = $${TARGET_INSTALL_PATH}/libs
        LIB_EXTENSION            = so*

        installtarget.path = $${TARGET_INSTALL_PATH}
        installtarget.files = $${PACKAGE_DESTDIR}/$${PACKAGE_TARGET}
        INSTALLS += installtarget
     } else:win32{
        message(Preparing installation for Win32)
        TARGET_INSTALL_PATH      = $${FMF_RELEASE_PACKAGES_PATH}/win/$${PACKAGE_TARGET}
        TARGET_BINARY            = $${TARGET_INSTALL_PATH}/$${PACKAGE_TARGET}
        PLUGINS_INSTALL_PATH     = $${TARGET_INSTALL_PATH}/plugins
        RESOURCES_INSTALL_PATH   = $${TARGET_INSTALL_PATH}/Resources
        LIBS_INSTALL_PATH        = $${TARGET_INSTALL_PATH}
        LIB_EXTENSION            = dll

        installtarget.path = $${TARGET_INSTALL_PATH}
        installtarget.files = $${PACKAGE_DESTDIR}/$${PACKAGE_TARGET}
        INSTALLS += installtarget
     }
 }
 TRANSLATIONS_INSTALL_PATH   = $${RESOURCES_INSTALL_PATH}/translations
 DATABASES_INSTALL_PATH      = $${RESOURCES_INSTALL_PATH}/databases
 PIXMAP_INSTALL_PATH         = $${RESOURCES_INSTALL_PATH}/pixmap
 FORMS_INSTALL_PATH          = $${RESOURCES_INSTALL_PATH}/forms
 QT_PLUGINS_INSTALL_PATH     = $${PLUGINS_INSTALL_PATH}/qt
 DOC_INSTALL_PATH            = $${RESOURCES_INSTALL_PATH}/doc/$${TARGET}/html
 message( Binary : )
 message(    * $${TARGET_INSTALL_PATH} )
 message( Application libraries : )
 message(    * From : $${PACKAGE_LIBS_BIN})
 message(    * To : $${LIBS_INSTALL_PATH})
 message(    * Extension : $${LIB_EXTENSION})
 message(    * RPath : $${RPATH_LIBS_BIN} )
 message( Plugins : )
 message(    * From : $${FMF_PLUGINS_DIR})
 message(    * To : $${PLUGINS_INSTALL_PATH} )
 message(    * Extension : $${LIB_EXTENSION})
 message( Qt Plugins : $${QT_PLUGINS_INSTALL_PATH} )
 message( Resources : $${RESOURCES_INSTALL_PATH} )
 message( Translations : $${TRANSLATIONS_INSTALL_PATH} )
 message( Database : $${DATABASES_INSTALL_PATH} )
 message( Pixmaps : $${PIXMAP_INSTALL_PATH} )
 message( Documentation : $${DOC_INSTALL_PATH} )

 # Install Application Libraries
 applibs.path = $${LIBS_INSTALL_PATH}
 applibs.files = $${PACKAGE_LIBS_BIN}/*.$${LIB_EXTENSION}
 INSTALLS += applibs

 # Install FMF plugins
 fmfplugins.path = $${PLUGINS_INSTALL_PATH}
 fmfplugins.files = $${FMF_PLUGINS_DIR}/*.$${LIB_EXTENSION}
 INSTALLS += fmfplugins

 # Install translations
 i18n.path = $${TRANSLATIONS_INSTALL_PATH}
 i18n.files = $${FMF_GLOBAL_RESOURCES}/translations/*.qm
 qti18n.path = $${TRANSLATIONS_INSTALL_PATH}
 qti18n.files = $$[QT_INSTALL_TRANSLATIONS]/translations/*fr.qm \
               $$[QT_INSTALL_TRANSLATIONS]/translations/*de.qm
 INSTALLS +=  qti18n i18n
# message( Application Translations will be installed into $${TRANSLATIONS_INSTALL_PATH} )
# message( Qt Translations will be installed from $$[QT_INSTALL_TRANSLATIONS] into $${TRANSLATIONS_INSTALL_PATH} )

 # FreeMedForms Specific resources
 !isEmpty(FMF_BUILD) {
    # Install forms
    fmfforms.path = $${FORMS_INSTALL_PATH}
    fmfforms.files = $${FMF_GLOBAL_RESOURCES}/forms
    INSTALLS += fmfforms
    message( Forms : $${FORMS_INSTALL_PATH} )
 }

 # Install databases
 !isEmpty(INSTALL_DRUGS) {
    drugsdatabases.path = $${DATABASES_INSTALL_PATH}/drugs
    drugsdatabases.files = $${FMF_GLOBAL_RESOURCES}/databases/drugs/drugs.db
    INSTALLS += drugsdatabases
 }
 usersdatabases.path = $${DATABASES_INSTALL_PATH}/users
 usersdatabases.files = $${FMF_GLOBAL_RESOURCES}/databases/users/*txt
 INSTALLS += usersdatabases

 # Install theme
 screens.path=$${PIXMAP_INSTALL_PATH}/splashscreens
 screens.files = $${FMF_GLOBAL_RESOURCES}/pixmap/splashscreens/*.png
 pix16.path = $${PIXMAP_INSTALL_PATH}/16x16
 pix16.files = $${FMF_GLOBAL_RESOURCES}/pixmap/16x16/*.png
 pix32.path = $${PIXMAP_INSTALL_PATH}/32x32
 pix32.files = $${FMF_GLOBAL_RESOURCES}/pixmap/32x32/*.png
 pix64.path = $${PIXMAP_INSTALL_PATH}/64x64
 pix64.files = $${FMF_GLOBAL_RESOURCES}/pixmap/64x64/*.png
 INSTALLS+=screens pix16 pix32 pix64

 # Documentation
 docs.path=$${DOC_INSTALL_PATH}
 docs.files=$${FMF_GLOBAL_RESOURCES}/doc/$${TARGET}/html/*
 INSTALLS+=docs

 # For linux copy needed qt libs
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
 } else {
      macx{
        QTLIBS_PATH = $$[QT_INSTALL_LIBS]
        QTPLUGINS_PATH = $$[QT_INSTALL_PLUGINS]
    #    qtlib.path  = $${TARGET_INSTALL_PATH}/Contents/FrameWorks
    #    qtlib.files = $$[QT_INSTALL_LIBS]
     } else:unix{
        QTLIBS_INSTALL_PATH=$${TARGET_INSTALL_PATH}/libs
        QTLIBS_PATH = $$[QT_INSTALL_LIBS]
        QTPLUGINS_PATH = $$[QT_INSTALL_PLUGINS]
        qtlibs.path  = $${QTLIBS_INSTALL_PATH}
        qtlibs.files = $$[QT_INSTALL_LIBS]/*QtCore.so.4 \
                       $$[QT_INSTALL_LIBS]/*QtGui.so.4 \
                       $$[QT_INSTALL_LIBS]/*QtSql.so.4 \
                       $$[QT_INSTALL_LIBS]/*QtScript.so.4 \
                       $$[QT_INSTALL_LIBS]/*Qt3Support.so.4 \
                       $$[QT_INSTALL_LIBS]/*QtXml.so.4 \
                       $$[QT_INSTALL_LIBS]/*QtSvg.so.4 \
                       $$[QT_INSTALL_LIBS]/*QtNetwork.so.4
        INSTALLS += qtlibs
        message( Unix : Qt Libs will be installed from $$[QT_INSTALL_LIBS] to $${QTLIBS_INSTALL_PATH} --$${TARGET_INSTALL_PATH}/libs)
     } else:win32{
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
 }

 # Install Qt plugins (SQL plugins)
 qt_sqlplugins.path = $${QT_PLUGINS_INSTALL_PATH}
 qt_sqlplugins.files = $${QTPLUGINS_PATH}/sqldrivers
 qt_imagesplugins.path = $${QT_PLUGINS_INSTALL_PATH}
 qt_imagesplugins.files = $${QTPLUGINS_PATH}/imageformats
 qt_accessibleplugins.path = $${QT_PLUGINS_INSTALL_PATH}
 qt_accessibleplugins.files = $${QTPLUGINS_PATH}/accessible
 INSTALLS += qt_sqlplugins qt_imagesplugins qt_accessibleplugins
 message( Qt Plugins will be installed from $${QTPLUGINS_PATH} to $${PLUGINS_INSTALL_PATH}/qt )

}  #end if release mode


