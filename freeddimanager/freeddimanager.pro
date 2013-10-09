include(../buildspecs/checkqtversion.pri)
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS = appnamedefine
# Do not build libs requiered
# (ex: linux integrated of apps dependent of the freemedforms-libs package)
!CONFIG(dontbuildlibs):SUBDIRS+=libs

SUBDIRS += plugins freeddimanager-src
