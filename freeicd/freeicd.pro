include(../buildspecs/checkqtversion.pri)
TEMPLATE=subdirs
CONFIG *= ordered

# Do not include libs if compiling linux_integrated & qmake requiered "onepasslibs"
# In this case libs are build by the freemedforms/freemedforms.pro project
CONFIG(LINUX_INTEGRATED):!CONFIG(onepasslibs):SUBDIRS=libs

SUBDIRS += plugins src
