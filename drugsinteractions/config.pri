# get current path
PACKAGE_PWD	= $$PWD
FMF_GLOBAL_RESOURCES  = $${PWD}/../global_resources

# package name
PACKAGE_TARGET = drugsinteractions
mac : PACKAGE_TARGET = $$quote(drugsinteractions)

# package destdir
PACKAGE_DESTDIR = $${PACKAGE_PWD}/bin

# temporary path for building
PACKAGE_BUILD_PATH  = $${PACKAGE_PWD}/../build
TOOLKIT_PATH        = $${PACKAGE_PWD}/../build

# include toolkit
INCLUDEPATH *= $${PWD}/../toolkit/sdk

# build mode
CONFIG              += qt warn_on thread x11 windows
mac:CONFIG          *= x86 ppc

# define config mode paths
CONFIG( debug, debug|release ) {
	#Debug
	message( Building in DEBUG )
	CONFIG	*= console
        mac:CONFIG -= ppc

        unix:PACKAGE_TARGET     = $$quote($$join(PACKAGE_TARGET,,,_debug))
        else:PACKAGE_TARGET     = $$quote($$join(PACKAGE_TARGET,,,d))
	unix:OBJECTS_DIR      = $${PACKAGE_BUILD_PATH}/debug/.obj/unix
	win32:OBJECTS_DIR     = $${PACKAGE_BUILD_PATH}/debug/.obj/win32
	mac:OBJECTS_DIR       = $${PACKAGE_BUILD_PATH}/debug/.obj/mac

	UI_DIR	= $${PACKAGE_BUILD_PATH}/debug/.ui
	MOC_DIR	= $${PACKAGE_BUILD_PATH}/debug/.moc
	RCC_DIR	= $${PACKAGE_BUILD_PATH}/debug/.rcc

        DEFINES *= DEBUG

} else {
	#Release
	message( Building in RELEASE )
	unix:OBJECTS_DIR     = $${PACKAGE_BUILD_PATH}/release/.obj/unix
	win32:OBJECTS_DIR    = $${PACKAGE_BUILD_PATH}/release/.obj/win32
	mac:OBJECTS_DIR	     = $${PACKAGE_BUILD_PATH}/release/.obj/mac
	UI_DIR	= $${PACKAGE_BUILD_PATH}/release/.ui
	MOC_DIR	= $${PACKAGE_BUILD_PATH}/release/.moc
	RCC_DIR	= $${PACKAGE_BUILD_PATH}/release/.rcc

        DEFINES *= QT_NO_DEBUG_OUTPUT \
                   QT_NO_WARNING_OUTPUT \
                   QT_NO_DEBUG
}

TARGET = $$PACKAGE_TARGET
DESTDIR = $$PACKAGE_DESTDIR

# define some usefull values
QMAKE_TARGET_COMPANY       = "FreeMedForms Team"
QMAKE_TARGET_PRODUCT       = "FreeMedForms Drugs Interactions Tester"
QMAKE_TARGET_DESCRIPTION   = "Standalone Drugs Interactions tester"
QMAKE_TARGET_COPYRIGHT     = "Copyright (C) 2008-2009 Eric MAEKER"
PACKAGE_DOMAIN             = "www.ericmaeker.fr/FreeMedForms"
PACKAGE_VERSION            = 0.05

# define macros for source code
DEFINES	*= "PACKAGE_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\"" \
           "PACKAGE_VERSION=\"\\\"$${PACKAGE_VERSION}\\\"\"" \
           "PACKAGE_DOMAIN=\"\\\"$${PACKAGE_DOMAIN}\\\"\"" \
           "PACKAGE_COPYRIGHTS=\"\\\"$${QMAKE_TARGET_COPYRIGHT}\\\"\"" \
           "FMF_GLOBAL_RESOURCES=\"\\\"$${FMF_GLOBAL_RESOURCES}\\\"\""


