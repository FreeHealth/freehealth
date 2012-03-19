include(../buildspecs/checkqtversion.pri)
TEMPLATE = subdirs

# Do not build libs requiered
# (ex: linux integrated of apps dependent of the freemedforms-libs package)
!CONFIG(dontbuildlibs):SUBDIRS=libs

SUBDIRS += plugins freeaccount-src
