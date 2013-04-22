#!/bin/bash

SCRIPT_NAME=`basename $0`
if [[ "`echo $0 | cut -c1`" = "/" ]]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

_P=`qmake -query QT_INSTALL_PREFIX`/mkspecs
_F=`ls $_P`
_O=$SCRIPT_PATH/../buildspecs/catchspec.pri
if [[ -f $_O ]]; then
    rm $_O
fi
touch $_O

echo "message(***  Catching the current specs)" > $_O

for f in $_F ; do
    if [[ "$f" != "common" && "$f" != "features" && "$f" != "default" ]]; then
        echo "$f{"
        echo "  message($f)"
        echo "  CURRENT_SPEC=$f"
        echo "}"
    fi
done >> $_O

exit 0
