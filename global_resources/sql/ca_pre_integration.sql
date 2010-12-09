-- $Source: create-ca.sql
-- $Revision: 1 $
-- Authors: Jim Busser, GNUmed project; Eric Maeker, FreeDiams project
-- script to create a sqlite db from Health Canada Drug Product Data
-- July 9, 2010 supports new (April 2010) HC drug field DESCRIPTOR VARCHAR2(150) 
-- July 9, 2010 bugfix: made the INSERT into DRUGS to be SELECT DISTINCT
-- NOTE: INSERT INTO "INFORMATIONS" ( needs manual updating as to
--   i)  the version of the drug db that it is to be known as
--   ii) the compatible version of FreeDiams


-- ****************************************************************
-- prepare to import, and import, the Canadian Drug Product Data  *
-- ****************************************************************
.mode csv
.separator "|"

-- note: DIN is short for DRUG_IDENTIFICATION_NUMBER
CREATE TABLE IF NOT EXISTS "drug" (
    "DRUG_CODE" INTEGER(8) NOT NULL ,
    "PRODUCT_CATEGORIZATION" VARCHAR2(80),
    "CLASS" VARCHAR2(40),
    "DIN" VARCHAR2(8),
    "BRAND_NAME" VARCHAR2(200),
    "DESCRIPTOR" VARCHAR2(150),
    "PEDIATRIC_FLAG" VARCHAR2(1),
    "ACCESSION_NUMBER" VARCHAR2(5),
    "NUMBER_OF_AIS" VARCHAR2(10),
    "LAST_UPDATE_DATE" DATE,
    "AI_GROUP_NO" VARCHAR2(10)
);


CREATE TABLE IF NOT EXISTS "form" (
    "DRUG_CODE" INTEGER(8) NOT NULL ,
    "PHARM_FORM_CODE" INTEGER(7),
    "PHARMACEUTICAL_FORM" VARCHAR2(40)
);

CREATE TABLE IF NOT EXISTS "route" (
    "DRUG_CODE" INTEGER(8)  NOT NULL ,
    "ROUTE_OF_ADMINISTRATION_CODE" INTEGER(6),
    "ROUTE_OF_ADMINISTRATION" VARCHAR2(40)
);

CREATE  TABLE IF NOT EXISTS "status" (
    "DRUG_CODE" INTEGER(8)  NOT NULL ,
    "CURRENT_STATUS_FLAG" VARCHAR2(1),
    "STATUS" VARCHAR2(40),
    "HISTORY_DATE" DATE
);

CREATE TABLE IF NOT EXISTS "ingred" (
    "DRUG_CODE" INTEGER(8) NOT NULL ,
    "ACTIVE_INGREDIENT_CODE" INTEGER(6),
    "INGREDIENT" VARCHAR2(240),
    "INGREDIENT_SUPPLIED_IND" VARCHAR2(1),
    "STRENGTH" VARCHAR2(20),
    "STRENGTH_UNIT" VARCHAR2(40),
    "STRENGTH_TYPE" VARCHAR2(40),
    "DOSAGE_VALUE" VARCHAR2(20),
    "BASE" VARCHAR2(1),
    "DOSAGE_UNIT" VARCHAR2(40),
    "NOTES" VARCHAR2(2000)
);


CREATE TABLE IF NOT EXISTS "ther" (
    "DRUG_CODE" INTEGER(8) NOT NULL ,
    "TC_ATC_NUMBER" VARCHAR2(8),
    "TC_ATC" VARCHAR2(120),
    "TC_AHFS_NUMBER" VARCHAR2(20),
    "TC_AHFS" VARCHAR2(80)
);

CREATE TABLE IF NOT EXISTS "package" (
    "DRUG_CODE" INTEGER(8) NOT NULL ,
    "UPC" VARCHAR2(12),
    "PACKAGE_SIZE_UNIT" VARCHAR2(40),
    "PACKAGE_TYPE" VARCHAR2(40),
    "PACKAGE_SIZE" VARCHAR2(5),
    "PRODUCT_INFORMATION" VARCHAR2(80)
);



-- the following depend on files fetched (and created) by download-and-create-ca.sh
-- reminder to self: SQLite has weak support of cross-table column value updating,
-- also certain unzip seems to output files into a subdirectory allfiles/
-- in this case enable the following instead:
-- .import allfiles/drug.csv drug
-- .import allfiles/form.csv form
-- .import allfiles/route.csv route
-- .import allfiles/status.csv status
-- .import allfiles/ingred.csv ingred
-- .import allfiles/ther.csv ther
-- .import allfiles/package.csv package

.import drug.csv drug
.import formshort.csv form
.import route.csv route
.import status.csv status
.import ingred.csv ingred
.import ther.csv ther
.import package.csv package

-- before implementing the following purge
-- must first delete corresponding records from other tables
-- DELETE FROM drug WHERE (CLASS = 'Veterinary');


ALTER TABLE "form" ADD "DIN" VARCHAR2(8);
ALTER TABLE "route" ADD "DIN" VARCHAR2(8);
ALTER TABLE "ingred" ADD "DIN" VARCHAR2(8);
ALTER TABLE "ther" ADD "DIN" VARCHAR2(8);
ALTER TABLE "package" ADD "DIN" VARCHAR2(8);

UPDATE form
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=form.DRUG_CODE
);

UPDATE route
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=route.DRUG_CODE
);

UPDATE ingred
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=ingred.DRUG_CODE
);

UPDATE ther
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=ther.DRUG_CODE
);

UPDATE package
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=package.DRUG_CODE
);



-- ********************************
-- Create *new* FreeDiams tables  *
-- ********************************
-- Create FreeDiams tables using the SVN sql code
--   http://code.google.com/p/freemedforms/source/browse/trunk/global_resources/sql/drugs_schema.sql
-- docs
--   http://ericmaeker.fr/FreeMedForms/di-manual/en/html/drugsdatabaseschema.html

CREATE TABLE IF NOT EXISTS "COMPOSITION" (
    "UID" varchar(20) NOT NULL,
    "MOLECULE_FORM" varchar(100),
    "MOLECULE_CODE" int(11) NOT NULL,
    "MOLECULE_NAME" varchar(200) NOT NULL,
    "DOSAGE" varchar(100)  NOT NULL,
    "DOSAGE_REF" varchar(50)  NOT NULL,
    "NATURE" varchar(2) NOT NULL DEFAULT "SA",
    "LK_NATURE" int(11) NOT NULL DEFAULT 1
);

-- CHECK definition on GLOBAL_STRENGTH


CREATE TABLE IF NOT EXISTS "DRUGS" (
    "UID" varchar(20) NOT NULL,
    "NAME" varchar(1000) NOT NULL,
    "FORM" varchar(500),
    "ROUTE" varchar(100),
    "ATC" varchar(7),
    "GLOBAL_STRENGTH" varchar(100),
    "TYPE_MP" varchar(1),
    "AUTHORIZATION" varchar(1),
    "MARKETED" bool NOT NULL DEFAULT 1,
    "LINK_SPC" varchar(250)
);

CREATE TABLE IF NOT EXISTS "INFORMATIONS" (
    "VERSION" varchar(10),
    "NAME" varchar(2000),
    "IDENTIFIANT" varchar(50),
    "COMPAT_VERSION" varchar(10),
    "PROVIDER" varchar(200),
    "WEBLINK" varchar(500),
    "COMPLEMENTARY_WEBSITE" varchar(200),
    "AUTHOR" varchar(200),
    "LICENSE" varchar(1000),
    "LICENSE_TERMS" varchar(10000),
    "DATE" varchar(20),
    "DRUG_UID_NAME" varchar(50),
    "PACK_MAIN_CODE_NAME" varchar(50),
    "ATC" bool NOT NULL,
    "INTERACTIONS" bool NOT NULL DEFAULT FALSE,
    "MOL_LINK_COMPLETION" integer DEFAULT 0,
    "AUTHOR_COMMENTS" varchar(10000),
    "LANGUAGE_COUNTRY" varchar(5),
    "DRUGS_NAME_CONSTRUCTOR" varchar(200)
);

CREATE TABLE IF NOT EXISTS "PACKAGING" (
    "UID" int(11) NOT NULL,
    "PACKAGE_UID" int(20) NOT NULL,
    "LABEL" varchar(500) NOT NULL,
    "STATUS" varchar(1),
    "MARKETING" varchar(1) NOT NULL DEFAULT 1,
    "DATE" varchar(25),
    "OPTIONAL_CODE" int(20)
);

CREATE TABLE IF NOT EXISTS "LK_MOL_ATC" (
    "MOLECULE_CODE" integer NOT NULL,
    "ATC" integer NOT NULL
);


-- feed table DRUGS
-- these are Branded products
-- the drugs must be distinct on {drug or combination} plus strength)
-- note FD's DRUGS table needs its records pre-ordered ASC on NAME

INSERT INTO DRUGS ("UID", "NAME")
SELECT DISTINCT
   A1.DIN,
   A1.BRAND_NAME
FROM drug A1
ORDER BY A1.BRAND_NAME;


-- account for products that package >1 form and/or can be used by multiple routes
UPDATE DRUGS
SET FORM=
(SELECT group_concat(PHARMACEUTICAL_FORM, ", ")
FROM form A1
WHERE A1.DIN=DRUGS.UID
GROUP BY DIN
LIMIT 10);

UPDATE DRUGS
SET ROUTE=
(SELECT group_concat(ROUTE_OF_ADMINISTRATION, ", ")
FROM route A1
WHERE A1.DIN=DRUGS.UID
GROUP BY DIN
LIMIT 10);

-- to adjust for the lack of strengths in some brand names

UPDATE DRUGS
SET GLOBAL_STRENGTH=
(SELECT group_concat(STRENGTH || STRENGTH_UNIT, ";")
FROM ingred A1
WHERE A1.DIN=DRUGS.UID
GROUP BY DIN
LIMIT 10);

-- set the ATC (CA "ther" table contains nondistinct ATC to support multiple AHFS records)

UPDATE DRUGS
SET ATC=
(SELECT DISTINCT TC_ATC_NUMBER
FROM ther A1
WHERE A1.DIN=DRUGS.UID
);



-- feed table COMPOSITION (molecular ingredients)
-- but without inserting "MOLECULE_FORM", 

INSERT INTO "COMPOSITION"
   ("UID", "MOLECULE_CODE", "MOLECULE_NAME", "DOSAGE", "DOSAGE_REF", "NATURE", "LK_NATURE")
SELECT DISTINCT
   DRUGS.UID,
   A1.ACTIVE_INGREDIENT_CODE,
   A1.INGREDIENT,
   A1.STRENGTH || A1.STRENGTH_UNIT || "/" || A1.DOSAGE_VALUE || A1.DOSAGE_UNIT,
   "",
   "SA",
   1
FROM DRUGS, ingred A1
WHERE
   (DRUGS.UID = A1.DIN) AND
   (A1.DOSAGE_VALUE != "")
   ;


INSERT INTO COMPOSITION
   ("UID", "MOLECULE_CODE", "MOLECULE_NAME", "DOSAGE", "DOSAGE_REF", "NATURE", "LK_NATURE")
SELECT DISTINCT
   DRUGS.UID,
   A1.ACTIVE_INGREDIENT_CODE,
   A1.INGREDIENT,
   A1.STRENGTH || A1.STRENGTH_UNIT,
   "",
   "SA",
   1
FROM DRUGS, ingred A1
WHERE
   (DRUGS.UID = A1.DIN) AND
   (A1.DOSAGE_VALUE = "");
   


-- Canada (2010-03) provides 1416 distinct 7-character ATCs for its drugs
-- 1164 of these are for single-active-ingredient drugs
-- another 318 are for multi-molecule drugs
--   e.g. UID 728 (molecules 6301 clidinium, 10260 chlordiazepoxide)
--   this has ATC A03CA02 meaning "clidinium and psycholeptics"
--   with a few exceptions, these ATCs may not be as badly needed.
-- also supplied are another 82 ATCs which are < 7-character
--
-- The following SQL will populate a link table with ~ 1394 records
-- having a 7-character ATC; these represent 1183 distinct molecules
-- out of the set of 3096 distinct molecules overall probably the main ones
-- (there exists an unexplained difference: 1183 distinct vs 1164 in source):

INSERT INTO LK_MOL_ATC (
    MOLECULE_CODE,
    ATC
    )
SELECT DISTINCT
    A1.MOLECULE_CODE,
    A2.ATC
FROM COMPOSITION A1, DRUGS A2, drug A3
WHERE A1.UID = A2.UID AND
    (A1.UID = A3.DIN AND A3.NUMBER_OF_AIS = 1 AND
    LENGTH(A2.ATC)=7);
    
-- to display ATC orphans e.g. CA molecule code 3123 UID 14681, 48348
--         or UID 15224 TICARCILLIN, molecule 19818	
-- SELECT  A1.UID, A1.MOLECULE_CODE, A1.MOLECULE_NAME
-- FROM COMPOSITION A1 LEFT JOIN MOLECULE_2_ATC A2 ON A1.MOLECULE_CODE = A2.MOLECULE_CODE
-- WHERE A2.ATC ISNULL
-- ORDER BY A1.MOLECULE_NAME


-- feed table INFORMATIONS (info about the drug data source)


INSERT INTO "INFORMATIONS" (
    "VERSION",
    "NAME",
    "IDENTIFIANT" ,
    "COMPAT_VERSION",
    "PROVIDER",
    "WEBLINK",
    "COMPLEMENTARY_WEBSITE",
    "AUTHOR",
    "LICENSE",
    "LICENSE_TERMS",
    "DATE",
    "DRUG_UID_NAME",
    "PACK_MAIN_CODE_NAME",
    "ATC",
    "INTERACTIONS",
    "MOL_LINK_COMPLETION",
    "AUTHOR_COMMENTS",
    "LANGUAGE_COUNTRY",
    "DRUGS_NAME_CONSTRUCTOR")
VALUES (
    "0.5.0",
    "en=Canadian Drug Product Database
    fr=Base de données thérapeutique Canadienne
    xx=Canadian Drug Product Database
    ",
    "CA_HCDPD",
    "0.5.0",
    "HC: Health Canada Drug Product Database",
    "http://www.hc-sc.gc.ca/dhp-mps/prodpharma/databasdon/index-eng.php",
    "http://code.google.com/p/freemedforms/wiki/Database_ca",
    "Jim Busser, MD & Eric Maeker, MD",
    "© Her Majesty the Queen in Right of Canada",
    "Apply: http://publications.gc.ca/helpAndInfo/cc-dac/application-e.html on a no-charge, royalty-free, non-exclusive, annual renewable basis.",
    CURRENT_DATE,
    "DRUG_CODE",
    "UPC",
    1,
    1,
    65,
    "While the download is technically open, the data is copyright. Please report any problem to the author.",
    "en_CA",
    "NAME (FORM, GLOBAL_STRENGTH)"
);


-- TABLE PACKAGING (available shipping sizes / quantities)
--
-- Eric offered the following but it does not take account of
--   variability in empty/non-empty fields within the source data
-- INSERT INTO PACKAGING
--    (`UID`, `PACKAGE_UID`, `LABEL`)
-- SELECT
--    `DRUG_CODE`, `UPC`, `PRODUCT_INFORMATION`
-- FROM
--    `package`;

INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
SELECT A7.DIN, A7.UPC, A7.PRODUCT_INFORMATION, " "
FROM package A7
WHERE PACKAGE_SIZE = "" AND PRODUCT_INFORMATION != "";

INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
SELECT A7.DIN, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE, " "
FROM package A7
WHERE PACKAGE_SIZE != "" AND PRODUCT_INFORMATION = "";

INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
SELECT A7.DIN, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE || ", " || PRODUCT_INFORMATION, " "
FROM package A7
WHERE PACKAGE_SIZE != "" AND PRODUCT_INFORMATION != "";

INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
SELECT A7.DIN, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE || ", " || PRODUCT_INFORMATION, " "
FROM package A7
WHERE PACKAGE_SIZE = "" AND PRODUCT_INFORMATION = "";

-- the Canadian products mostly lack any unique UPC
-- a pseudo PACKAGE_UID is possible, but might be inconstant
UPDATE package SET UPC = ROWID;



-- uncomment the following to automatically drop the DPD staging tables
-- DROP TABLE drug;
-- DROP TABLE form;
-- DROP TABLE form-abbr;
-- DROP TABLE route;
-- DROP TABLE status;
-- DROP TABLE ingred;
-- DROP TABLE ther;
-- DROP TABLE package;


.exit

-- the following are handled by Eric's constructors

CREATE TABLE IF NOT EXISTS "SEARCH_ENGINES" ( 
  `ID` integer primary key, 
  `LABEL` varchar(25), 
  `URL` varchar(1000) 
);

CREATE TABLE "DB_SCHEMA_VERSION" ( 
  `VERSION` varchar(10), 
  `DATE`  date, 
  `COMMENT` varchar(500) 
);

.import SEARCH_ENGINES.csv SEARCH_ENGINES
.import DB_SCHEMA_VERSION.csv DB_SCHEMA_VERSION





-- the remainder of this file (below .exit) are notes kept for reference

-- SELECTS to examine data
-- SELECT A1.NAME, A2.MOLECULE_NAME, A1.ATC, A3.TC_ATC
-- FROM DRUGS A1, COMPOSITION A2, ther A3
-- WHERE A1.UID = A2.UID AND A1.UID = A3.DRUG_CODE
-- returns after nearly 5-10 minutes 52,635 columns
-- DRUGS 23,124 columns
-- COMPO 38,883
-- THER 16,965
-- some which lack ATCs are homeopathic products



-- SQlite Date And Time Functions
-- http://www.sqlite.org/cvstrac/wiki/wiki?p=DateAndTimeFunctions
-- SELECT (CURRENT_DATE) is same as SELECT (date('now'))

-- *************************************
-- Deprecated
-- *************************************

-- INSERT INTO COMPOSITION (UID, MOLECULE_FORM, MOLECULE_CODE, MOLECULE_NAME, DOSAGE, DOSAGE_REF, NATURE)
-- SELECT A5.DRUG_CODE, A2.FORM_SHORT, A5.ACTIVE_INGREDIENT_CODE, A5.INGREDIENT, A5.STRENGTH || " " || A5.STRENGTH_UNIT, "per " || A2.FORM_SHORT, A1.NUMBER_OF_AIS
-- FROM ingred A5, form A2, drug A1
-- WHERE A5.DRUG_CODE = A2.DRUG_CODE AND A5.DRUG_CODE = A1.DRUG_CODE AND A5.DOSAGE_VALUE !="" ;
-- 
-- INSERT INTO COMPOSITION (UID, MOLECULE_FORM, MOLECULE_CODE, MOLECULE_NAME, DOSAGE, DOSAGE_REF, NATURE)
-- SELECT A5.DRUG_CODE, A2.FORM_SHORT, A5.ACTIVE_INGREDIENT_CODE, A5.INGREDIENT, A5.STRENGTH || " " || A5.STRENGTH_UNIT, "per " || A2.FORM_SHORT, A1.NUMBER_OF_AIS
-- FROM ingred A5, form A2, drug A1
-- WHERE A5.DRUG_CODE = A2.DRUG_CODE AND A5.DRUG_CODE = A1.DRUG_CODE AND A5.DOSAGE_VALUE ="" ;

-- INSERT INTO DRUGS (UID, NAME, FORM, ROUTE)
-- SELECT A1.DRUG_CODE, A1.BRAND_NAME, A2.FORM_SHORT, A3.ROUTE_OF_ADMINISTRATION
-- FROM drug A1, form A2, route A3
-- WHERE A1.DRUG_CODE = A2.DRUG_CODE AND A2.DRUG_CODE = A3.DRUG_CODE
-- ORDER BY A1.BRAND_NAME;
-- 
-- UPDATE DRUGS
-- SET ATC = (SELECT A4.TC_ATC_NUMBER
-- FROM ther A4
-- WHERE DRUGS.UID = A4.DRUG_CODE);

-- CREATE TABLE "form" ("DRUG_CODE" INTEGER(8) NOT NULL , "PHARM_FORM_CODE" INTEGER(7), "PHARMACEUTICAL_FORM" VARCHAR2(40));
-- CREATE TABLE "form_abbr" ("DRUG_CODE" INTEGER(8) NOT NULL , "PHARM_FORM_CODE" INTEGER(7), "FORM_ABBR" VARCHAR2(40));
-- .import form.csv form
-- .import form-abbr.csv form_abbr
--
-- provide an abbreviation and secondary key to give uniqueness in the FreeDiams molecule form
-- ALTER TABLE "form" ADD "FORM_ABBR" VARCHAR2(40);
--
-- SELECT REPLACE(A1.FORM_ABBR,'%',A2.FORM_ABBR)
-- FROM form A1, form_abbr A2
-- WHERE A1.DRUG_CODE = A2.DRUG_CODE AND A1.PHARM_FORM_CODE = A2.PHARM_FORM_CODE;

-- JB experimental
UPDATE DRUGS
SET DRUG_CODE=UID, DIN=
(SELECT DISTINCT
   A1.DRUG_IDENTIFICATION_NUMBER || ";" || A2.PHARM_FORM_CODE || ";" || A3.ROUTE_OF_ADMINISTRATION_CODE
FROM drug A1, form A2, route A3
WHERE
   (A2.DRUG_CODE = A1.DRUG_CODE) AND
   (A3.DRUG_CODE = A1.DRUG_CODE) AND
   (A1.DRUG_CODE=DRUGS.UID)
);

