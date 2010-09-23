-- /***************************************************************************
-- *   FreeMedicalForms                                                      *
-- *   (C) 2008-2010 by Eric MAEKER, MD                                      *
-- *   eric.maeker@free.fr                                                   *
-- *   All rights reserved.                                                  *
-- *                                                                         *
-- *   This program is a free and open source software.                      *
-- *   It is released under the terms of the new BSD License.                *
-- *                                                                         *
-- *   Redistribution and use in source and binary forms, with or without    *
-- *   modification, are permitted provided that the following conditions    *
-- *   are met:                                                              *
-- *   - Redistributions of source code must retain the above copyright      *
-- *   notice, this list of conditions and the following disclaimer.         *
-- *   - Redistributions in binary form must reproduce the above copyright   *
-- *   notice, this list of conditions and the following disclaimer in the   *
-- *   documentation and/or other materials provided with the distribution.  *
-- *   - Neither the name of the FreeMedForms' organization nor the names of *
-- *   its contributors may be used to endorse or promote products derived   *
-- *   from this software without specific prior written permission.         *
-- *                                                                         *
-- *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
-- *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
-- *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
-- *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
-- *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
-- *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
-- *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
-- *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
-- *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
-- *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
-- *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
-- *   POSSIBILITY OF SUCH DAMAGE.                                           *
-- ***************************************************************************/
-- /**************************************************************************
-- *   Main Developper : Jim Busser, MD <jbusser@interchange.ubc.ca>         *
-- *   Contributors :                                                        *
-- *       Eric MAEKER, MD <eric.maeker@free.fr>                             *
-- ***************************************************************************/

-- /**
--  * \file canadian_db_finalize.sql
--  * \author Jim Busser, MD <jbusser@interchange.ubc.ca>, Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.4.4
--  * \date 14 July 2010
--  */

-- /**
--  * \brief This script creates the FreeDiams Canadian's drugs database itself.
--  *
--  * \warning SQL commands MUST end with \e ;
--  *
--  * Sept 22, 2010 : recreate the drug's UID using forms and routes ID.
--  *   Preparation of the future surrogate key.
--  *   Drugs are created by the FreeToolBox code.
--  *
--  * July 14, 2010 initial release
--  *
--  * NOTE: INSERT INTO "INFORMATIONS" (needs manual updating as to
--  *  i)  the version of the drug db that it is to be known as
--  *  ii) the compatible version of FreeDiams
--  *  iii) manual insertion into LK_MOL_ATC until scriptable
--  */


-- ********************************
-- Feed FreeDiams tables
-- ********************************
-- feed table DRUGS
-- these are Branded products
-- the drugs must be distinct on {drug or combination} plus strength
-- note FreeDiam's DRUGS table needs its records pre-ordered ASC on NAME

-- INSERT INTO DRUGS ("UID", "NAME", "FORM", "ROUTE", "ATC")
-- SELECT DISTINCT
--    A1.DRUG_CODE,
--    A1.BRAND_NAME,
--    A2.PHARMACEUTICAL_FORM,
--    A3.ROUTE_OF_ADMINISTRATION,
--    A4.TC_ATC_NUMBER
-- FROM drug A1, form A2, route A3, ther A4
-- WHERE
--    (A2.DRUG_CODE = A1.DRUG_CODE) AND
--    (A3.DRUG_CODE = A1.DRUG_CODE) AND
--    (A4.DRUG_CODE = A1.DRUG_CODE)
-- ORDER BY A1.BRAND_NAME;

-- to adjust for the lack of strengths in some brand names
-- UPDATE DRUGS
-- SET GLOBAL_STRENGTH =
--   (
--     SELECT group_concat(STRENGTH || STRENGTH_UNIT, ";")
--     FROM ingred
--     WHERE DRUG_CODE=DRUGS.UID
--     GROUP BY DRUG_CODE
--     LIMIT 10
--   );


-- feed table COMPOSITION (molecular ingredients)
-- INSERT INTO `COMPOSITION`
--    (`UID`, `MOLECULE_FORM`, `MOLECULE_CODE`, `MOLECULE_NAME`, `DOSAGE`)
--   SELECT
--     A1.DRUG_CODE,
--     A2.PHARMACEUTICAL_FORM,
--     A1.ACTIVE_INGREDIENT_CODE,
--     A1.INGREDIENT,
--     A1.STRENGTH || A1.STRENGTH_UNIT || "/" || A1.DOSAGE_VALUE || A1.DOSAGE_UNIT
--   FROM ingred A1, form A2
--   WHERE
--     (A1.DRUG_CODE = A2.DRUG_CODE) AND
--     (A1.DOSAGE_VALUE != "");


-- INSERT INTO `COMPOSITION`
--    (`UID`, `MOLECULE_FORM`, `MOLECULE_CODE`, `MOLECULE_NAME`, `DOSAGE`)
--   SELECT
--     A1.DRUG_CODE,
--     A2.PHARMACEUTICAL_FORM,
--     A1.ACTIVE_INGREDIENT_CODE,
--     A1.INGREDIENT,
--     A1.STRENGTH || A1.STRENGTH_UNIT
--   FROM ingred A1, form A2
--   WHERE
--     (A1.DRUG_CODE = A2.DRUG_CODE) AND
--     (A1.DOSAGE_VALUE = "");

-- TO DO -- need suitable insert from CSV or other source
-- INSERT INTO "LK_MOL_ATC" (
--     MOLECULE_CODE,
--     ATC_ID
--     )


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
    "AUTHOR_COMMENTS",
    "LANGUAGE_COUNTRY",
    "DRUGS_NAME_CONSTRUCTOR")
VALUES (
    "0.5.0",
    "xx=Canadian Drug Product Database
    fr=Base de données thérapeutique Canadienne
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
SELECT A7.DRUG_CODE, A7.UPC, A7.PRODUCT_INFORMATION, " "
FROM package A7
WHERE PACKAGE_SIZE = "" AND PRODUCT_INFORMATION != "";

INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
SELECT A7.DRUG_CODE, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE, " "
FROM package A7
WHERE PACKAGE_SIZE != "" AND PRODUCT_INFORMATION = "";

INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
SELECT A7.DRUG_CODE, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE || ", " || PRODUCT_INFORMATION, " "
FROM package A7
WHERE PACKAGE_SIZE != "" AND PRODUCT_INFORMATION != "";

INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
SELECT A7.DRUG_CODE, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE || ", " || PRODUCT_INFORMATION, " "
FROM package A7
WHERE PACKAGE_SIZE = "" AND PRODUCT_INFORMATION = "";


-- the Canadian products mostly lack any unique UPC
-- a pseudo PACKAGE_UID is possible, but might be inconstant
UPDATE package SET UPC = ROWID;

-- Drop the DPD staging tables
DROP TABLE drug;
DROP TABLE form;
DROP TABLE route;
DROP TABLE status;
DROP TABLE ingred;
DROP TABLE ther;
DROP TABLE package;

--DROP TABLE form-abbr;

VACUUM;

-- Kept for reference

-- SELECTS to examine data
-- SELECT A1.NAME, A2.MOLECULE_NAME, A1.ATC, A3.TC_ATC
-- FROM DRUGS A1, COMPOSITION A2, ther A3
-- WHERE A1.UID = A2.UID AND A1.UID = A3.DRUG_CODE
-- returns after nearly 5-10 minutes 52,635 rows
-- DRUGS 23,124 rows
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
