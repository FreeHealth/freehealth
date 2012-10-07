#!/bin/bash
# $Source: localize-bc_CA
# $Revision: 1 $
# Author: Jim Busser, GNUmed and FreeDiams projects
# script to obtain BC.CA drug authorization data by which to feed FreeDiams
# overview:
# - clone (copy) FreeDiams drugs-en_CA.db into ~/tmp and then
# - download BC Ministry of Health Services' Drug Data Files
# - inflate and extract desired values, pipe-separated
#     (pipe-separated believed more-reliably parsed by sqlite3)

# in case ~/tmp directory does not yet exist…
mkdir ~/tmp
echo "Working in ~/tmp/ ..."

# assume we are working from a Mac OS-installed drugs-en_CA.db
cp /Applications/FreeDiams.app/Contents/Resources/databases/drugs/drugs-en_CA.db ~/tmp/
cp ../../../global_resources/sql/drugdb/ca/localize-bc_CA.sql ~/tmp/
cd ~/tmp/


# **************************************************
# obtain data files from BC Ministry of Health
echo "Obtaining data..."

# PharmaNet Drug Master
wget http://www.health.gov.bc.ca/pharmacare/outgoing/pnp.zip

# Alternative Drug Categories
wget http://www.health.gov.bc.ca/pharmacare/outgoing/drugmast.zip

# not yet needed
# Reference Drug Program (RDP) Categories
# wget http://www.health.gov.bc.ca/pharmacare/outgoing/rdpcateg.zip

# no longer available
# Low Cost Alternative Drug Categories
# wget http://www.health.gov.bc.ca/pharmacare/outgoing/lca.zip

# **********************************************************************
# unzip them and remove the archives
# note: unzip *.zip may not work
# **********************************************************************
unzip pnp.zip
unzip drugmast.zip

# **********************************************************************
# Reprocess each source file in turn, using the sTREAM edITOR 'sed':
# **********************************************************************
# each one requires, as a first step, removal of Windows' end-of-line CR
# detailed example of sed usage is porvided below, under # NOTES


# PharmaNet Drug Master
# pnp.txt
# ---keep---
# CDIC - Canadian Drug Identification Number
# LCA - “F” indicates a full LCA benefit, “P” indicates a partial LCA benefit
# TRIAL
# MAXPRIC - PharmaCare maximum price.
# GENPRIC - An entry in this field other than zero indicates that the drug is a partial benefit under the Low Cost Alternative (LCA) Program and the LCA price indicated is the highest amount PharmaCare will recognize for the drug. Zero indicates drug is a full benefit under the LCA Program or is not included in the LCA Program.
# DAYPRIC - Reference drug price per day. Does not apply to entries showing 9901 in the RBPCAT column.
# ----------
# sed -e 's/.$//' -e 's:^\(.\{8\}\)\(.\{26\}\)\(.\):\1\|\3\|:' -e 's:^\(.\{11\}\)\(.\{85\}\)\(.\{9\}\)\(..\):\1\3\|:' -e 's:^\(.\{29\}\)\(.\{14\}\)\(.\{8\}\)\(...\):\1\|\3\|:' -e 's:^\(.\{38\}\)\(.*\):\1:' pnp.txt >pnp.csv
sed -e 's/.$//' -e 's:^\(.\{8\}\)\(.\{26\}\)\(.\):\1\|\3\|:' -e 's:^\(.\{11\}\)\(.\{83\}\)\(.\{1\}\):\1\3\|:' -e 's:^\(.\{13\}\)\(.\{1\}\)\(.\{9\}\)\(..\):\1\3\|:' -e 's:^\(.\{31\}\)\(.\{14\}\)\(.\{8\}\)\(...\):\1\|\3\|:' -e 's:^\(.\{40\}\)\(.*\):\1:' pnp.txt >pnp.csv

# Alternative Drug Categories
# drugmast.txt
# ---keep---
# ALT	"A" indicates a generic drug that is a partial benefit.
# CDIC	Canadian Drug Identification Number
# BEN	Benefit group(s) under which the drug is eligible
# ----------
# drop leading blank; keep ALT, drop blank; keep 10 chars, drop blank, keep 16
# ----------
sed -e 's/.$//' -e 's:^ ::' -e 's:^\(.\)\(.\)\(.*\):\1\|\3:' -e 's:^\(.\{10\}\)\(.\{1\}\)\(.\{16\}\)\(.*\):\1\|\3:' drugmast.txt >drugmast.csv

# *************************************************
# Import BC data into staging tables in drugs-en_CA
# *************************************************

echo "Raw data preparation complete..."
echo "Updating CA database with BC data."
echo "Completion may take 2 minutes..."
# call with < (calling with -init seemingly ignores .exit)
sqlite3 drugs-en_CA.db <localize-bc_CA.sql

# ********************************
# Clean up
# ********************************

echo "Cleaning up..."
# cleanup
rm pnp.*
rm drugmast.*

exit

# END
# ************************************************************************************

# **********************
# NOTES
# **********************
# Reprocess each source file in turn, using the sTREAM edITOR 'sed':
#
# here is detailed sed syntax as used below to parse the PharmaNet Drug Master
# 
# -->
#   remove the Windows end-of-line CR
#   sed -e 's/.$//' 
#
# for each of the below, beginning at start-of-line ^
#   using the any-character designator "."
#   using escaped (\) braces ({}) for positional substitution
#   do as follows
# -->
#   designate the first 8 characters (CDIC = DIN number) as "part 1"
#   designate the next, undesired characters (26) as "part 2"
#   designate the next, desired character (LCA length 1) as "part 3"
#   output "part 1" concatenated with pipe "|" then "part 3" concatenated with pipe "|"
#   -e 's:^\(.\{8\}\)\(.\{26\}\)\(.\):\1\|\3\|:'
# -->
#   DEPRECATE
#   designate the first 11 characters (CDIC, pipe, LCA, pipe) as "part 1" for retention
#   designate the next undesired characters (85) as "part 2"
#   designate the next desired characters (MAXPRIC length 9) as "part 3" and drop the trailing BLANK(2)
#   output "part 1" (already has an end-pipe) concatenated with "part 3" and pipe "|"
#   -e 's:^\(.\{11\}\)\(.\{85\}\)\(.\{9\}\)\(..\):\1\3\|:'
#   PATCH to preserve TRIAL info, need to change to 83, keep 1, discard 1
#   designate the next undesired characters (83) as "part 2"
#   designate the next desired character (TRIAL length 1) as "part 3"
#   output "part 1" (already has an end-pipe) concatenated with "part 3" and pipe "|"
#   -e 's:^\(.\{11\}\)\(.\{83\}\)\(.\{1\}\):\1\3\|:'
#   AND
#   designate the first 13 characters (CDIC, pipe, LCA, pipe, Trial, pipe) as "part 1" for retention
#   designate the next undesired 1 character (BLANK) as "part 2"
#   designate the next desired 9 characters (MAXPRIC) as "part 3" and drop the trailing BLANK(2)
#   output "part 1" (already has an end-pipe) concatenated with "part 3" and pipe "|"
#   -e 's:^\(.\{13\}\)\(.\{1\}\)\(.\{9\}\)\(..\):\1\3\|:'
# -->
#   DEPRECATE
#   designate the first 29 characters (21 above plus adjacent 8 for GENPRIC) as "part 1"
#   drop the next 14, keep 8 (DAYPRIC), strip 3 (preserves UNITPRIC in the remnant)
#   -e 's:^\(.\{29\}\)\(.\{14\}\)\(.\{8\}\)\(...\):\1\|\3\|:'
# -->
#   PATCH
#   designate the first 31 characters (23 above plus adjacent 8 for GENPRIC) as "part 1"
#   drop the next 14 (part 2), keep 8 (DAYPRIC, part 3), drop BLANK(3) in case later
#   desire UNITPRIC in remnant)
#   -e 's:^\(.\{31\}\)\(.\{14\}\)\(.\{8\}\)\(...\):\1\|\3\|:'
# -->
#   DEPRECATE
#   desire first 38 characters from output: CDIC|LCA|MAXPRIC|GENPRIC|DAYPRIC
#   designate the remainder of the line (.*\) as "part 2"
#   note: the above took the decision that we are not interested in UNITPRIC
#   -e 's:^\(.\{38\}\)\(.*\):\1:'
# -->
#   PATCH
#   desire first 40 characters from output: CDIC|LCA|MAXPRIC|GENPRIC|DAYPRIC
#   designate the remainder of the line (.*\) as "part 2"
#   note: the above took the decision that we are not interested in UNITPRIC
#   -e 's:^\(.\{40\}\)\(.*\):\1:'
#   
# sample iterative results beginning *after* the second -e:
#   
#   -e 's:^\(.\{8\}\)\(.\{26\}\)\(.\):\1\|\3\|:'
#   00000019| |   PLACIDYL CAP 200MG                   003525 ETHCHLORVYNOL 200 MG CA             N 0000.3640  000.0000              000.0000   000.0000            
#   
#   -e 's:^\(.\{11\}\)\(.\{85\}\)\(.\{9\}\)\(..\):\1\3\|:'
#   00000019| |0000.3640|000.0000              000.0000   000.0000
#   
#   -e 's:^\(.\{29\}\)\(.\{14\}\)\(.\{8\}\)\(...\):\1\|\3\|:'
#   00000019| |0000.3640|000.0000|000.0000|000.0000            
#   
#   -e 's:^\(.\{38\}\)\(.*\):\1:'
#   00000019| |0000.3640|000.0000|000.0000


# **********************************************************************
# file:
# PharmaNet Drug Master
# pnp.txt
# -----------------------
# This file lists all drugs that are PharmaCare benefits and their benefit groups.
# A generic price of $0 indicates the drug is (a) a full benefit under the Low Cost Alternative (LCA) Program or (b) is not included in the LCA program.
# -----------------------
# Variable Descriptions
# -----------------------
# CDIC - Canadian Drug Identification Number
# BENGRP - Benefit group(s) under which the drug is eligible
# LCA - “F” indicates a full LCA benefit, “P” indicates a partial LCA benefit
# BRANDNM - Brand name of the drug
# MANUF - Manufacturer of the drug
# CHEMNAME - Active ingredient of the drug
# TRIAL - “Y”= drug is eligible for Trial Prescription Program
# MAXPRIC - PharmaCare maximum price.
# GENPRIC - An entry in this field other than zero indicates that the drug is a partial benefit under the Low Cost Alternative (LCA) Program and the LCA price indicated is the highest amount PharmaCare will recognize for the drug. Zero indicates drug is a full benefit under the LCA Program or is not included in the LCA Program.
# RBPCAT - Reference drug category. Note: Entries showing 9901 are not RDP categories.
# SUBCAT - Reference drug sub category
# UNITDAY - Unit of measure used for Reference Drug Program pricing (U=Unit, D=Day)
# DAYPRIC - Reference drug price per day. Does not apply to entries showing 9901 in the RBPCAT column.
# UNITPRIC - Reference drug price per unit. This field is no longer used.
# EXCLPLAN - Plan(s) under which the drug is not eligible for coverage. If the RBPCAT column is populated, ignore plans shown in this column.
# 
# File Layout
# -----------------------------
# VARIABLE DATA TYPE POSITIONS
# CDIC	INTEGER	1 - 8
# BLANK	 	9
# BENGRP	CHAR	10 - 34
# LCA	CHAR	35
# BLANK	 	36 - 38
# BRANDNM	CHAR	39 - 73
# BLANK	 	74 - 75
# MANUF	CHAR	76 - 81
# BLANK	 	82
# CHEMNAME	CHAR	83 - 117
# BLANK	 	118
# TRIAL	CHAR	119
# BLANK	 	120
# MAXPRIC	DEC	121 - 129
# BLANK	 	130 - 131
# GENPRIC	DEC	132 - 139
# BLANK	 	140 - 142
# RBPCAT	CHAR	143 - 146
# SUBCAT	CHAR	147 - 150
# BLANK	 	151
# UNITDAY	CHAR	152
# BLANK	 	153
# DAYPRIC	DEC	154 - 161
# BLANK	 	162 - 164
# UNITPRIC	DEC	165 - 172
# BLANK	 	173 - 175
# EXCLPLAN	CHAR	176 - 185



# **********************************************************************
# file:
# Alternative Drug Categories
# drugmast.txt
# ------------------
# VARIABLE	DESCRIPTION
# ALT	"A" indicates a generic drug that is a partial benefit.
# CDIC	Canadian Drug Identification Number
# BEN	Benefit group(s) under which the drug is eligible
# DRUGNM	Brand name of the drug
# MNFCTRR	Manufacturer of the drug
# BRAND	"Y" indicates a brand drug, "N" indicates a generic
# CHEMNAME	Active ingredient of the drug
# CLASS	American Hospital Formulary Service Pharmacologic Therapeutic Classification
# ------------------
# VARIABLE DATA TYPE POSITIONS
# ------------------
# BLANK	 	1
# ALT	CHAR	2
# BLANK	 	3
# CDIC	INTEGER	4 - 11
# BLANK	 	12
# BEN	CHAR	13 - 28
# BLANK	 	29
# DRGNM	CHAR	30 - 64
# BLANK	 	65 - 69
# MNFCTRR	CHAR	70 - 75
# BLANK	 	76
# BRAND	CHAR	77
# BLANK	 	78
# CHEMNAME	CHAR	79 - 113
# BLANK	 	114
# THCL	CHAR	115 - 127


# **********************************************************************
# file:
# file no longer being provided by Medical Services
# Low Cost Alternative Drug Categories
# lca.txt
# ------------------
# 
# --------------------------------------
# Variable Descriptions
# CDIC	Canadian Drug Identification Number
# EFF_DATE	Effective date of the drug price
# LCA_PRICE	Highest per unit price that PharmaCare pays/paid for this drug.
# FULLBEN	File contains only partial benefits (indicated by “P”)
# BONLY	“B” indicates coverage is available only for Plan B patients
# RBPCAT	Reference Drug Program category
# SUBCAT	Reference Drug Program sub category
# UNITDAY	Unit measure used for RDP pricing (D=daily, U=unit)
# RBPPRICE	Reference drug price
# EXCLPLAN	PharmaCare plan(s) under which the drug is not a benefit
# CNCL_DT	Cancellation date of price
# 
# File Layout
# VARIABLE DATA TYPE POSITIONS
# CDIC	CHAR	1 - 8
# EFF_DATE	CHAR	9 - 16
# LCA_PRICE	CHAR	17 - 23
# BLANK	CHAR	24
# FULL_BEN	CHAR	25 - 27
# PLANB	CHAR	28
# RBPCAT 	CHAR	29 - 32
# SUBCAT	CHAR	33 - 36
# UNITDAY 	CHAR	37
# RBPPRICE	CHAR	38 - 44
# EXCLPLAN	CHAR	45 - 104
# CNCL_DT 	CHAR	105 - 112

# DEPRECATED
# file no longer being provided by Medical Services
# Low Cost Alternative Drug Categories
# lca.txt
# ---keep---
# CDIC	Canadian Drug Identification Number
# LCA_PRICE	Highest per unit price that PharmaCare pays/paid for this drug.
# FULLBEN	File contains only partial benefits (indicated by “P”)
# ----------
# sed -e 's/.$//' -e 's:\(........\)\(........\)\(.......\)\(.\):\1\|\3\|:' -e 's:\(..................\)\(...................\)\(.*\):\1\|\2:' lca.txt >lca.csv



# **********************************************************************
# file:
# Reference Drug Program (RDP) Categories
# rdpcateg.txt
# ------------------
# 
# --------------------------------------
# VARIABLE	DESCRIPTION
# RBPCAT	Reference Drug Program category
# SUBCAT	Reference Drug Program sub category
# SHRTDESC	Short description of drug
# LONGDESC	Long description of drug
# EXCLPLAN	PharmaCare plan(s) under which drug is not eligible for coverage.
# UNITDAY	Daily versus unit cost flag. Unit measure used for RDP pricing (U=Unit, D=Daily)
# DAYPRIC	RDP per day "9999.9999" in this column indicates the drug requires PharmaCare Special Authority.
# UNITPRIC	This field is no longer used.
# EFFDATE	Effective date of price.
# 
# File Layout
# VARIABLE	DATA TYPE	POSITIONS
# RBPCAT	CHAR	1 - 4
# SUBCAT	CHAR	5 - 8
# SHRTDESC	CHAR	9 - 38
# LONGDESC	CHAR	39 - 88
# BLANK	 	89
# EXCLPLAN	CHAR	90 - 96
# BLANK	 	97 - 99
# UNITDAY	CHAR	100
# BLANK	 	101
# DAYPRIC	DEC	102 - 111
# UNITPRIC	DEC	112 - 121
# EFFDATE	INTEGER	122 - 129
# BLANK	 	130


