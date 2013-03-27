#include(../../../plugins/fmf_plugins.pri)
isEmpty(TEMPLATE):TEMPLATE=app

QT += testlib sql
CONFIG += qt warn_on console depend_includepath testcase
CONFIG -= app_bundle


# prefix test binary with tst_
!contains(TARGET, ^tst_.*):TARGET = $$join(TARGET,,"tst_")
