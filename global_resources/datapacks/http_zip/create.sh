#!/bin/sh

# Get MD5 of pack files
# Get size of pack files
# Replace in pack description files

sys=`uname -s`
if [ "$sys" == "Linux" ] ; then
    md5="md5sum"
    statarg="-c%s"
    sedbak=""
elif [ "$sys" == "Darwin" ] ; then
    md5="md5 -q"
    statarg="-s"
    sedbak=".bak"
fi

echo "md5 command will be $md5"

xmlfiles=`find . -type f \( -iname "*.xml" ! -iname "server.conf.xml" \)`
# Get all xml files excepted server.conf.xml
for xmlfile in $xmlfiles
do
    zipdir=`dirname $xmlfile`
    zipfile=`ls $zipdir/*.zip | head -1` # we assume that the first zip file in the directory is the good one

    if [[ ! -e $zipfile ]]; then
        echo "No zip file found for $xmlfile"
        continue;
    fi

    echo "Preparing $xmlfile."
    echo "  . Zip: $zipfile"

    # Compute data to insert
    zipmd5=$($md5 $zipfile | cut -d ' ' -f 1)
    if [ "$sys" == "Linux" ] ; then
        zipsize=$(stat $statarg "$zipfile")
    elif [ "$sys" == "Darwin" ] ; then
        zipsize=`du -k $zipfile | awk '{print $1}'`
    fi

    echo "  . md5: $zipmd5"
    echo "  . size: $zipsize"

    # Proceed to the replacement
    sed -i $sedbak "s/<size>.*<\/size>/<size>$zipsize<\/size>/g" $xmlfile
    sed -i $sedbak "s/<md5>.*<\/md5>/<md5>$zipmd5<\/md5>/g" $xmlfile
done

# Create the zipped server configuration
rm serverconf.zip
zip serverconf.zip server.conf.xml $xmlfiles

exit 0
