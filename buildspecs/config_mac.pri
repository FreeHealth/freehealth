macx{
   # Qt 4.7 does not support ppc by default
   #CONFIG *= x86 x86-64
   CONFIG  -= ppc
   QMAKE_MACOSX_DEPLOYMENT_TARGET=10.5

   # define rpath
   CONFIG(debug_without_install) {
     RPATH_LIBS_BIN   = ../../../plugins/
   } else {
     RPATH_LIBS_BIN   = ../plugins/
   }
}
