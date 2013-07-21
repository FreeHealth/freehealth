#!/bin/sh
for i in $( ls plugins );
do
echo
echo "********* Realease translation for plugin : "$i
echo
echo `lrelease plugins/$i/*plugin.pro`
done

for i in $( ls libs );
do
echo
echo "********* Realease translation for libs : "$i
echo
echo `lrelease libs/$i/*utils.pro`
done
