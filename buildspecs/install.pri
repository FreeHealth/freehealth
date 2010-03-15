# define some default paths for installation process
mac:INSTALL_BASENAME_PATH          = mac
else:linux*:INSTALL_BASENAME_PATH  = linux
else:win32:INSTALL_BASENAME_PATH   = win

# Qt libs and plugs
INSTALL_QT_LIBS_PATH      = $${INSTALL_BINARY_PATH}/libs
INSTALL_QT_PLUGINS_PATH   = $${INSTALL_PLUGINS_PATH}/qt

# some help for file copying
LIB_EXTENSION             = so*

# These inclusions modify the default path for the installation process
macx:include(install_mac.pri)
win32:include(install_win.pri)
linux*:include(install_linux.pri)

# by default package is constructed inside the svn trunk under
# packages/yourOs/Application
isEmpty(INSTALL_ROOT_PATH):     INSTALL_ROOT_PATH      = $${RELEASE_BINARY_PATH}/$${INSTALL_BASENAME_PATH}/$${BINARY_TARGET}
isEmpty(INSTALL_BINARY_PATH):   INSTALL_BINARY_PATH    = $${INSTALL_ROOT_PATH}
isEmpty(INSTALL_LIBS_PATH):     INSTALL_LIBS_PATH      = $${INSTALL_BINARY_PATH}
isEmpty(INSTALL_PLUGINS_PATH):  INSTALL_PLUGINS_PATH   = $${INSTALL_LIBS_PATH}
isEmpty(INSTALL_RESOURCES_PATH):INSTALL_RESOURCES_PATH = $${INSTALL_BINARY_PATH}/Resources
isEmpty(INSTALL_DESKTOP_FILES_PATH):INSTALL_DESKTOP_FILES_PATH =
isEmpty(INSTALL_DESKTOP_ICON_PATH):INSTALL_DESKTOP_ICON_PATH=

DEFINES *= "INSTALL_BINARY_PATH=\"\\\"$$INSTALL_BINARY_PATH\\\"\"" \
           "INSTALL_LIBS_PATH=\"\\\"$$INSTALL_LIBS_PATH\\\"\"" \
           "INSTALL_PLUGINS_PATH=\"\\\"$$INSTALL_PLUGINS_PATH\\\"\"" \
           "INSTALL_RESOURCES_PATH=\"\\\"$$INSTALL_RESOURCES_PATH\\\"\"" \
	   "INSTALL_DESKTOP_FILES_PATH=\"\\\"$$INSTALL_DESKTOP_FILES_PATH\\\"\"" \
	   "INSTALL_QT_PLUGINS_PATH=\"\\\"$$INSTALL_QT_PLUGINS_PATH\\\"\""

# resources paths
INSTALL_TRANSLATIONS_PATH = $${INSTALL_RESOURCES_PATH}/translations
INSTALL_DATABASES_PATH    = $${INSTALL_RESOURCES_PATH}/databases
INSTALL_FORMS_PATH        = $${INSTALL_RESOURCES_PATH}
INSTALL_SMALLPIX_PATH     = $${INSTALL_RESOURCES_PATH}/pixmap/16x16
INSTALL_MEDIUMPIX_PATH    = $${INSTALL_RESOURCES_PATH}/pixmap/32x32
INSTALL_BIGPIX_PATH       = $${INSTALL_RESOURCES_PATH}/pixmap/64x64
INSTALL_SPLASHPIX_PATH    = $${INSTALL_RESOURCES_PATH}/pixmap/splashscreens
INSTALL_DOCS_PATH         = $${INSTALL_RESOURCES_PATH}/doc/$${BINARY_TARGET}

# install Qt libs and plugins inside the bundle ; leave it empty if you don't want to install QT libs and plugs inside the bundle
INSTALL_QT_INSIDE_BUNDLE = true

# message the configuration
message( Binary : )
message(    * From : $${BUID_BINARY_PATH} )
message(    * To : $${INSTALL_BINARY_PATH} )
message( Application libraries : )
message(    * From : $${BUILD_LIB_PATH})
message(    * To : $${INSTALL_LIBS_PATH})
message(    * Extension : $${LIB_EXTENSION})
message(    * RPath : $${RPATH_LIBS_BIN} )
message( Plugins : )
message(    * From : $${BUILD_PLUGIN_PATH})
message(    * To : $${INSTALL_PLUGINS_PATH} )
message(    * Extension : $${LIB_EXTENSION})
!isEmpty(INSTALL_QT_LIBS_PATH):message( Qt Libs : $${INSTALL_QT_LIBS_PATH} )
!isEmpty(INSTALL_QT_PLUGINS_PATH):message( Qt Plugins : $${INSTALL_QT_PLUGINS_PATH} )
message( Resources : $${INSTALL_RESOURCES_PATH} )
message( Translations : $${INSTALL_TRANSLATIONS_PATH} )
!isEmpty(INSTALL_DATABASES_PATH):message( Database : $${INSTALL_DATABASES_PATH} )
!isEmpty(INSTALL_FORMS_PATH):message( Forms : $${INSTALL_FORMS_PATH} )
message( Pixmaps - small : $${INSTALL_SMALLPIX_PATH} )
message( Pixmaps - medium : $${INSTALL_MEDIUMPIX_PATH} )
message( Pixmaps - big : $${INSTALL_SBIGPIX_PATH} )
message( Pixmaps - splashscreens : $${INSTALL_SPLASHPIX_PATH} )
!isEmpty(INSTALL_DOCS_PATH):message( Documentation : $${INSTALL_DOCS_PATH} )
!isEmpty(INSTALL_DESKTOP_FILES_PATH):message( DesktopFile : $${INSTALL_DESKTOP_FILES_PATH} )
!isEmpty(INSTALL_DESKTOP_ICON_PATH):message( DesktopIcon : $${INSTALL_DESKTOP_ICON_PATH} )

# Install target
!isEmpty(INSTALL_BINARY_PATH):!isEmpty(BUID_BINARY_PATH){
target.path = $${INSTALL_BINARY_PATH}
#installtarget.files = $${BUID_BINARY_PATH}/$${BINARY_TARGET}
INSTALLS += target
}

# Install libs
!isEmpty(INSTALL_LIBS_PATH):!isEmpty(BUILD_LIB_PATH){
applibs.path = $${INSTALL_LIBS_PATH}
applibs.files = $${BUILD_LIB_PATH}/*.$${LIB_EXTENSION}
applibs.CONFIG += no_check_exist
INSTALLS += applibs
}
# Install plugins
!isEmpty(INSTALL_PLUGINS_PATH):!isEmpty(BUILD_PLUGIN_PATH){
plugs.path = $${INSTALL_PLUGINS_PATH}
plugs.files = $${BUILD_PLUGIN_PATH}/*.$${LIB_EXTENSION}
plugs.CONFIG += no_check_exist
INSTALLS += plugs
}
# Install plugins specs
!isEmpty(INSTALL_PLUGINS_PATH):!isEmpty(BUILD_PLUGIN_PATH){
plugs_specs.path = $${INSTALL_PLUGINS_PATH}
plugs_specs.files = $${BUILD_PLUGIN_PATH}/*.pluginspec
plugs_specs.CONFIG += no_check_exist
INSTALLS += plugs_specs
}

# Install translations
!isEmpty(INSTALL_TRANSLATIONS_PATH):!isEmpty(SOURCES_TRANSLATIONS){
i18n.path = $${INSTALL_TRANSLATIONS_PATH}
i18n.files = $${SOURCES_TRANSLATIONS}/*.qm
qti18n.path = $${INSTALL_TRANSLATIONS_PATH}
qti18n.files = $$[QT_INSTALL_TRANSLATIONS]/translations/*fr.qm \
              $$[QT_INSTALL_TRANSLATIONS]/translations/*de.qm
INSTALLS +=  qti18n i18n
}

# Install forms
!isEmpty(INSTALL_FORMS_PATH):!isEmpty(SOURCES_FORMS){
forms.path = $${INSTALL_FORMS_PATH}
forms.files = $${SOURCES_FORMS}
INSTALLS += forms
}

# Install databases
!isEmpty(INSTALL_DATABASES_PATH):!isEmpty(SOURCES_DATABASES){
drugsdb.path = $${INSTALL_DATABASES_PATH}/drugs
drugsdb.files = $${SOURCES_DATABASES}/drugs/drugs*.db
INSTALLS += drugsdb
usersdb.path = $${INSTALL_DATABASES_PATH}/users
usersdb.files = $${SOURCES_GLOBAL_RESOURCES}/databases/users/*txt
INSTALLS += usersdb
}

# Install theme
screens.path = $${INSTALL_SPLASHPIX_PATH}
screens.files = $${SOURCES_GLOBAL_RESOURCES}/pixmap/splashscreens/*.png
pix16.path = $${INSTALL_SMALLPIX_PATH}
pix16.files = $${SOURCES_GLOBAL_RESOURCES}/pixmap/16x16/*.png
pix32.path = $${INSTALL_MEDIUMPIX_PATH}
pix32.files = $${SOURCES_GLOBAL_RESOURCES}/pixmap/32x32/*.png
pix64.path = $${INSTALL_BIGPIX_PATH}/64x64
pix64.files = $${SOURCES_GLOBAL_RESOURCES}/pixmap/64x64/*.png
pixsvg.path = $${INSTALL_BIGPIX_PATH}/svg
pixsvg.files = $${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/*.svg
INSTALLS+=screens pix16 pix32 pix64

# Install desktop file
!isEmpty(INSTALL_DESKTOP_FILES_PATH){
desk.path=$${INSTALL_DESKTOP_FILES_PATH}
desk.files=$${SOURCES_GLOBAL_RESOURCES}/textfiles/$${TARGET}.desktop
desk_icon.path=$${INSTALL_DESKTOP_ICON_PATH}
desk_icon.files=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/$${TARGET}.svg
INSTALLS+= desk desk_icon
}

# Documentation
!isEmpty(INSTALL_DOCS_PATH){
docs.path=$${INSTALL_DOCS_PATH}
docs.files=$${SOURCES_GLOBAL_RESOURCES}/doc/$${TARGET}
docs.CONFIG += no_check_exist
INSTALLS+=docs
}

# configuration for non-integrated solutions (everything is included inside the bundle)
!isEmpty(INSTALL_QT_INSIDE_BUNDLE){
   message($${INSTALL_QT_INSIDE_BUNDLE})
   QTLIBS_INSTALL_PATH=$${INSTALL_QT_LIBS_PATH}
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
   message( Qt Libs will be installed from $$[QT_INSTALL_LIBS] to $${QTLIBS_INSTALL_PATH})
   # Install Application Libraries

   # Install Qt plugins (SQL plugins)
   qt_sqlplugins.path = $${INSTALL_QT_PLUGINS_PATH}
   qt_sqlplugins.files = $${QTPLUGINS_PATH}/sqldrivers
   qt_imagesplugins.path = $${INSTALL_QT_PLUGINS_PATH}
   qt_imagesplugins.files = $${QTPLUGINS_PATH}/imageformats
   qt_accessibleplugins.path = $${INSTALL_QT_PLUGINS_PATH}
   qt_accessibleplugins.files = $${QTPLUGINS_PATH}/accessible
   INSTALLS += qt_sqlplugins qt_imagesplugins qt_accessibleplugins
   message( Qt Plugins will be installed from $${QTPLUGINS_PATH} to $${INSTALL_QT_PLUGINS_PATH} )
  }

