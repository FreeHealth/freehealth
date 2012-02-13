#!/bin/sh

# Get MD5 of pack files
# Get size of pack files
# Replace in pack description files

# Create the zipped server configuration
rm serverconf.zip
zip serverconf.zip server.conf.xml icd10/icd10.xml drugs/all/drugs.xml drugs/fr-only/drugs.xml

exit 0
