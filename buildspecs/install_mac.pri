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
 !macx:error(install_mac.pri should only be used when building Mac version)
 message( ***************************************************** )
 message( *************** INSTALLATION PROCESS **************** )
 message( ***************************************************** )

 # Install binary
 message(Preparing installation for Mac X)
 INSTALL_ROOT_PATH          = $${BINARY_RELEASE_PATH}/mac/$${PACKAGE_TARGET}
 INSTALL_BINARY_TARGET_PATH = $${INSTALL_ROOT_PATH}/$${PACKAGE_TARGET}
 INSTALL_PLUGINS_PATH       = $${INSTALL_ROOT_PATH}/$${PACKAGE_TARGET}.app/Contents/plugins
 INSTALL_RESOURCES_PATH     = $${INSTALL_ROOT_PATH}/$${PACKAGE_TARGET}.app/Contents/Resources
 INSTALL_LIBS_PATH          = $${INSTALL_ROOT_PATH}/$${PACKAGE_TARGET}.app/Contents/plugins
 LIB_EXTENSION              = $${QMAKE_EXTENSION_SHLIB}

 target.path = $${INSTALL_ROOT_PATH}
 #target.files = $${PACKAGE_DESTDIR}/$${PACKAGE_TARGET}
 INSTALLS += target

 TRANSLATIONS_INSTALL_PATH   = $${INSTALL_RESOURCES_PATH}/translations
 DATABASES_INSTALL_PATH      = $${INSTALL_RESOURCES_PATH}/databases
 PIXMAP_INSTALL_PATH         = $${INSTALL_RESOURCES_PATH}/pixmap
 FORMS_INSTALL_PATH          = $${INSTALL_RESOURCES_PATH}/forms
 QT_INSTALL_PLUGINS_PATH     = $${INSTALL_PLUGINS_PATH}/qt
 DOC_INSTALL_PATH            = $${INSTALL_RESOURCES_PATH}/doc/$${TARGET}/html
 message( Binary : )
 message(    * $${INSTALL_ROOT_PATH} )
 message( Application libraries : )
 message(    * From : $${PACKAGE_LIBS_BIN})
 message(    * To : $${INSTALL_LIBS_PATH})
 message(    * Extension : $${LIB_EXTENSION})
 message(    * RPath : $${RPATH_LIBS_BIN} )
 message( Plugins : )
 message(    * From : $${FMF_PLUGINS_DIR})
 message(    * To : $${INSTALL_PLUGINS_PATH} )
 message(    * Extension : $${LIB_EXTENSION})
 message( Qt Plugins : $${QT_INSTALL_PLUGINS_PATH} )
 message( Resources : $${INSTALL_RESOURCES_PATH} )
 message( Translations : $${TRANSLATIONS_INSTALL_PATH} )
 message( Database : $${DATABASES_INSTALL_PATH} )
 message( Pixmaps : $${PIXMAP_INSTALL_PATH} )
 message( Documentation : $${DOC_INSTALL_PATH} )

 # Install Application Libraries
 applibs.path = $${INSTALL_LIBS_PATH}
 applibs.files = $${PACKAGE_LIBS_BIN}/*.$${LIB_EXTENSION}
 INSTALLS += applibs

 # Install FMF plugins
 fmfplugins.path = $${INSTALL_PLUGINS_PATH}
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

 QTLIBS_PATH = $$[QT_INSTALL_LIBS]
 QTPLUGINS_PATH = $$[QT_INSTALL_PLUGINS]

 # Install Qt plugins (SQL plugins)
 qt_sqlplugins.path = $${QT_INSTALL_PLUGINS_PATH}
 qt_sqlplugins.files = $${QTPLUGINS_PATH}/sqldrivers
 qt_imagesplugins.path = $${QT_INSTALL_PLUGINS_PATH}
 qt_imagesplugins.files = $${QTPLUGINS_PATH}/imageformats
 qt_accessibleplugins.path = $${QT_INSTALL_PLUGINS_PATH}
 qt_accessibleplugins.files = $${QTPLUGINS_PATH}/accessible
 INSTALLS += qt_sqlplugins qt_imagesplugins qt_accessibleplugins
 message( Qt Plugins will be installed from $${QTPLUGINS_PATH} to $${INSTALL_PLUGINS_PATH}/qt )

}  #end if release mode


