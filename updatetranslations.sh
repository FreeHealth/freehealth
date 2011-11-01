#!/bin/sh
for i in $( ls plugins );
do
echo
echo "********* Updating translation for FreeMedForms plugin : "$i
echo
echo `lupdate plugins/$i/*plugin.pro -no-obsolete`
done

for i in $( ls freediams/plugins );
do
echo
echo "********* Updating translation for FreeDiams plugin : "$i
echo
echo `lupdate freediams/plugins/$i/*.pro -no-obsolete`
done

for i in $( ls libs );
do
echo
echo "********* Updating translation for libs : "$i
echo
echo `lupdate libs/$i/*.pro -no-obsolete`
done

