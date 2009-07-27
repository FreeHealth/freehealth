#This is the include file to add to your plugins project

# set plugin name define
!isEmpty( TARGET ):DEFINES *= "PLUGIN_NAME=\"\\\"$${TARGET}\\\"\""

# include config file
include( $${PWD}/../../config.pri )

# include freemedforms framework
# include( $${PACKAGE_PWD}/freemedforms/freemedforms.pri )

TEMPLATE	= lib
CONFIG          *= dll plugin
PRE_TARGETDEPS	*= $${PACKAGE_PWD}/freemedforms

mac:*-g++:LIBS  *= -Wl,-undefined,dynamic_lookup

# ubuntu hardy/debian fix
QMAKE_LFLAGS	-= -Wl,--no-undefined
win32:LIBS           += -l$${FREEMEDFORMS}
