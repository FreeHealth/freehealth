TEMPLATE	= lib

# set plugin name define
!isEmpty( TARGET ):DEFINES *= "PLUGIN_NAME=\"\\\"$${TARGET}\\\"\""

# include config file
include( $${PWD}/../../config.pri )

CONFIG          *= dll plugin

DEFINES         *= PLUGIN_BUILD

PRE_TARGETDEPS	*= $${PACKAGE_PWD}/freemedforms

DESTDIR          = $${FMF_PLUGINS_DIR}
mac:*-g++:LIBS  *= -Wl,-undefined,dynamic_lookup

# ubuntu hardy/debian fix
QMAKE_LFLAGS	-= -Wl,--no-undefined

win32:LIBS      += -l$${FREEMEDFORMS}
