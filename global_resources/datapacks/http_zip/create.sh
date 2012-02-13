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
    md5="md5"
    statarg="-s"
    sedbak=".bak"
fi

echo "md5 command will be $md5"

# Get all xml files excepted server.conf.xml
for xmlfile in `find . -type f \( -iname "*.xml" ! -iname "server.conf.xml" \)`
do
    zipdir=`dirname $xmlfile`
    zipfile=`ls $zipdir/*.zip | head -1` # we assume that the first zip file in the directory is the good one

    # Compute data to insert
    zipmd5=$($md5 $zipfile | cut -d ' ' -f 1)
    if [ "$sys" == "Linux" ] ; then
        zipsize=$(stat $statarg "$zipfile")
    elif [ "$sys" == "Darwin" ] ; then
        zipsize=`du -k $zipfile | awk '{print $1}'`
    fi

    echo "For $zipfile, md5=$zipmd5 and size=$zipsize"

    # Proceed to the replacement
    sed -i $sedbak "s/<size>.*<\/size>/<size>$zipsize<\/size>/g" $xmlfile
    sed -i $sedbak "s/<md5>.*<\/md5>/<md5>$zipmd5<\/md5>/g" $xmlfile
done

# Create the zipped server configuration
rm serverconf.zip
zip serverconf.zip server.conf.xml icd10/icd10.xml drugs/all/drugs.xml drugs/fr-only/drugs.xml

exit 0
