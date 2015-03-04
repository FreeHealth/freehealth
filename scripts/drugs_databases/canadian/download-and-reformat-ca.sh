#!/bin/bash
# $Source: download-and-reformat-ca.sh
# $Revision: 2 $
# Author: Jim Busser, GNUmed project
# script to
# - download Health Canada Drug Product Databases (DPD)
# - inflate and convert CSV to pipe-separated values
#   more easily parsed by sqlite3
# - call sqlite to create a db suitable for use with FreeDiams

#echo "Creating temporary directory ~/tmp/drugsca..."
rm -R ~/tmp/drugsca
mkdir ~/tmp/
mkdir ~/tmp/drugsca
cp -R ../../../global_resources/sql/ ~/tmp/drugsca
cp ../../../global_resources/sql/drugs_schema.sql ~/tmp/drugsca/drugdb/ca/
# files:
# cp ../../../global_resources/sql/atc_utf8.csv
# cp ../../../global_resources/sql/drugs_schema.sql
# cp ../../../global_resources/sql/iam_schema.sql
# cp ../../../global_resources/sql/canadian*.*

cd ~/tmp/drugsca/drugdb/ca/


#obtain *licensed* data files from Health Canada
echo "Obtaining data..."
wget http://www.hc-sc.gc.ca/dhp-mps/prodpharma/databasdon/txt/allfiles.zip
unzip *.zip
rm *.zip

# process some (but not all) DPD files, since FreeDiams does not yet use them all

sed -e 's/.$//' -e 's/","/|/g' -e 's/\(^"\)\(.*\)/\2/' -e 's/"$//' drug.txt >drug.csv
sed -e 's/.$//' -e 's/","/|/g' -e 's/\(^"\)\(.*\)/\2/' -e 's/"$//' form.txt >form.csv
sed -e 's/.$//' -e 's/","/|/g' -e 's/\(^"\)\(.*\)/\2/' -e 's/"$//' route.txt >route.csv
sed -e 's/.$//' -e 's/","/|/g' -e 's/\(^"\)\(.*\)/\2/' -e 's/"$//' status.txt >status.csv
sed -e 's/.$//' -e 's/","/|/g' -e 's/\(^"\)\(.*\)/\2/' -e 's/"$//' ingred.txt >ingred.csv
sed -e 's/.$//' -e 's/","/|/g' -e 's/\(^"\)\(.*\)/\2/' -e 's/"$//' ther.txt >ther.csv
sed -e 's/.$//' -e 's/","/|/g' -e 's/\(^"\)\(.*\)/\2/' -e 's/"$//' package.txt >package.csv

# NOTES:
# the source files are Windows CR/LF format files
# also, SQLite CLI sometimes has trouble parsing csv ("", "") import
# For this reason, Stream EDiting (sed) was used to substitute vertical pipes.
# This relies on the absence of vertical pipes in the source!
# 
# EXPLANATION:
# employ the -e 'multiple command option' for 4 commands
#
# 's/.$//'
# --> uses the sed 'substitution' (s) command
# --> on account of these being Windows CR/LF files:
# --> substitutes the last character '.' prior to line endings '$' with nothing
#
# 's/","/|/g'
# --> replaces the csv format 'comma-within-double quotes' "," with a vertical pipe
# --> fortunately, this drug data source quoted-wrapped nontext data as well
# --> non-universally quote-wrapped CSVs would need a more complex regex to catch [0-9]*,
# --> recursively (globally: g) for all occurrences within each line
#
# 's/\(^"\)\(.*\)/\2/'
# --> looks for two patterns, each within escaped parentheses \( \)
# --> first pattern is beginning of line double-quote ^"
# --> second pattern is the remainder of the line
# --> only the second among these patterns \2 is retained
#
# 's/"$//'
# --> remove the trailing quote " before end of line $
# 
# file1 >file2
# --> specifies the input and output files
# --> the alternative within-command 'write to file'
#     's/foo/bar/w file2'
#     is messy on account of echoing all lines to stdout
# --> do not bother with option -n as that results in empty files


# ****** patch DPD bug ******
# a fix for inappropriate % in source gas products in drug.txt
# example from the source file (post-substitution of pipes "|")
# 13920||Human|02014424|%CARBON MONOX.%HELIUM%OXYGEN%NITROGEN L.D.M.||N|75399|4|25-SEP-2009|0415738002
mv drug.csv drug.buggy
sed -e 's:\([^%].*\)\(|%\)\([A-Z]\):\1|\3:' -e 's:\([^%]*\)\(%\)\([A-Z]\):\1 \3:g' drug.buggy >drug.csv
# ********************


# now also create a drug 'form' abbreviation that can double as short key
# this will recursively (':::g' argument) replace space-parens, parens, comma space, hyphens with " "# and will re-output form.csv as form.tmp containing only the PHARM_FORM_CODE and last field
# note the ':::g w outfile' option seemed to write incomplete files, hence w is replaced by >outfile
sed -e 's: (: :g' -e 's:): :g' -e 's:-: :g' -e 's:, : :g' form.csv >form2.csv
# it will then reprocess the resulting file
# pre-empt otherwise-duplicate key, and replace words 2-5 with concat (_ & words' leading characters)
sed -e 's:\([0-9]*[|]\)\(POWDER FOR SOLUTION\):\1POWDER 4 S:' -e 's:\([0-9]*|[A-Z]*\)\( [.]*\):\1_\2:' -e 's:\( \)\([A-Z0-9]\)\([A-Z0-9]*\):\2:g' form2.csv >formshort.csv

# deprecated
# remove the first pipe-delimited field
# sed 's:\([0-9]*[|]\)\([0-9]*[|]\)\([.]*\):\2\3:w outfile' infile
# old: sed -e 's:\([0-9]*[|]\)\(POWDER FOR SOLUTION\):\1POWDER 4 S:' -e 's:\([A-Za-z]* \)\([A-Z]\)\([A-Z]* \)\([A-Z]\)\([A-Z]* \)\([A-Z]\)\([A-Z]* \)\([A-Z]\)\([A-Z]*\):\1\2 \4 \6 \8:' -e 's:\([A-Za-z]*\)\( \)\([.]*\):\1_\3:w form-abbr.csv' form.tmp

echo "Raw data preparation should be complete..."
echo "Moving on to create the drugs database..."

sqlite3 drugs-en_CA.db <canadian_db_creator.sql

mv drugs-en_CA.db ~/tmp

echo "Check line(s) above for errors. If none, we maybe succeeded."

# TODO prompt user whether to preserve ~/tmp/drugsca for debugging
#rm -R ~/tmp/drugsca

echo "Cleaning up..."
echo "Please look inside user directory ~/tmp for 'drugs-en_CA.db'!"
echo "Please also bear in mind that more values may be needed for LK_MOL_ATC."
open ~/tmp
