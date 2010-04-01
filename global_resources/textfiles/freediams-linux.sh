#!/bin/sh
# Binary wrapper for FreeDiams Linux release

#appname=`basename $0 | sed s,\.sh$,,`
appname=`basename $0`.bin

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

qtlibspath=$dirname/libs

LD_LIBRARY_PATH=$qtlibspath
export LD_LIBRARY_PATH

$dirname/$appname $*
