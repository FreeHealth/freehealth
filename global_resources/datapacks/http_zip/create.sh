#!/bin/sh

# Get MD5 of pack files
# Get size of pack files
# Replace in pack description files

sys=`uname -s`
if [ "$sys" == "Linux" ] ; then
    md5cmd="md5sum"
elif [ "$sys" == "Darwin" ] ; then
    md5cmd="md5"
fi

# Get all xml files excepted server.conf.xml
for xmlfile in `find . -type f \( -iname "*.xml" ! -iname "server.conf.xml" \)`
do
    zipdir=`dirname $xmlfile`
    zipfile=`ls $zipdir/*.zip | head -1` # we assume that the first zip file in the directory is the good one

    # Compute data to insert
    zipmd5=$($md5cmd $zipfile | cut -d ' ' -f 1)
    if [ "$sys" == "Linux" ] ; then
        zipsize=$(stat -c%s "$zipfile")
    elif [ "$sys" == "Darwin" ] ; then
        zipsize=$(stat -s "$zipfile")
    fi

    # Proceed to the replacement
    sed -i "s/<size>.*<\/size>/<size>$zipsize<\/size>/g" $xmlfile
    sed -i "s/<md5>.*<\/md5>/<md5>$zipmd5<\/md5>/g" $xmlfile
done

# Create the zipped server configuration
rm serverconf.zip
zip serverconf.zip server.conf.xml icd10/icd10.xml drugs/all/drugs.xml drugs/fr-only/drugs.xml

exit 0
