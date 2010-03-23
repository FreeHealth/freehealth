#!/bin/sh
for i in $( ls plugins );
do
echo
echo "********* Updating translation for plugin : "$i
echo
echo `lupdate plugins/$i/*plugin.pro -no-obsolete`
done

for i in $( ls libs );
do
echo
echo "********* Updating translation for libs : "$i
echo
echo `lupdate libs/$i/*utils.pro -no-obsolete`
done

