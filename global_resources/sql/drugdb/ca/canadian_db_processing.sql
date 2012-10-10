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
--  * \file canadian_db_processing.sql
--  * \author Jim Busser, MD <jbusser@interchange.ubc.ca>, Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.4.4
--  * \date 14 July 2010
--  */

-- /**
--  * \brief This script populates the FreeDiams Canadian's drugs database from source tables.
--  *
--  * \warning SQL commands MUST end with \e ;
--  *
--  * Dec 26, 2010
--  *   More correctly constructed values for global_strength and dosage
--  *   Patched also against Canadian source bug (drugs missing ingredients)
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
-- Remove Veterinary drugs
-- ********************************
-- do the following for each of tables
-- form
-- route
-- ingred
-- ther
-- package

DELETE FROM drug
WHERE Class = "Veterinary"
;


-- ********************************
-- Feed FreeDiams table DRUGS
-- ********************************
-- these are Branded products
-- the drugs must be distinct on {drug or combination} plus strength)
-- note FreeDiam's DRUGS table needs its records pre-ordered ascending on column NAME

INSERT INTO DRUGS ("UID", "NAME")
SELECT DISTINCT
   A1.DRUG_CODE,
   A1.BRAND_NAME
FROM drug A1
ORDER BY A1.BRAND_NAME;


-- account for products that package >1 form and/or can be used by multiple routes
UPDATE DRUGS
SET FORM=
(SELECT group_concat(PHARMACEUTICAL_FORM, ", ")
FROM form A1
WHERE A1.DRUG_CODE=DRUGS.UID
GROUP BY DRUG_CODE
LIMIT 10);

UPDATE DRUGS
SET ROUTE=
(SELECT group_concat(ROUTE_OF_ADMINISTRATION, ", ")
FROM route A1
WHERE A1.DRUG_CODE=DRUGS.UID
GROUP BY DRUG_CODE
LIMIT 10);

-- ensure that any Canadian drug NULL strengths are backfilled from related ingredients
-- update is performed on two subsets of records, on account of non-uniform strength formulae

UPDATE DRUGS
SET GLOBAL_STRENGTH=
(SELECT group_concat(STRENGTH || A1.STRENGTH_UNIT || "/" || A1.DOSAGE_VALUE || A1.DOSAGE_UNIT, ";")
FROM ingred A1
WHERE A1.DRUG_CODE=DRUGS.UID AND A1.DOSAGE_UNIT != "" AND
   (A1.DOSAGE_VALUE || A1.DOSAGE_UNIT != A1.STRENGTH_UNIT)
GROUP BY DRUG_CODE
LIMIT 10)
WHERE GLOBAL_STRENGTH IS NULL
;

UPDATE DRUGS
SET GLOBAL_STRENGTH=
(SELECT group_concat(STRENGTH || A1.STRENGTH_UNIT, ";")
FROM ingred A1
WHERE A1.DRUG_CODE=DRUGS.UID AND (A1.DOSAGE_UNIT = "" OR
   A1.DOSAGE_VALUE || A1.DOSAGE_UNIT = A1.STRENGTH_UNIT)
GROUP BY DRUG_CODE
LIMIT 10)
WHERE GLOBAL_STRENGTH IS NULL
;

-- set the ATC
-- note Canada's "ther" table contains nondistinct ATCs (to support multiple AHFS values)

UPDATE DRUGS
SET ATC=
(SELECT DISTINCT TC_ATC_NUMBER
FROM ther A1
WHERE A1.DRUG_CODE=DRUGS.UID
);

-- patch error in Canadian DPD source (Dec 2010) where 27 drugs lacked ingredients

DELETE
FROM DRUGS
WHERE GLOBAL_STRENGTH IS NULL
;

-- ********************************
-- Feed FreeDiams table COMPOSITION (molecular ingredients)
-- ********************************
-- note: some have DOSAGE_VALUE, others do not
-- also omitting in this step to populate "MOLECULE_FORM"

INSERT INTO "COMPOSITION"
   ("UID", "MOLECULE_CODE", "MOLECULE_NAME", "DOSAGE", "DOSAGE_REF", "NATURE", "LK_NATURE")
SELECT
   A1.DRUG_CODE,
   A1.ACTIVE_INGREDIENT_CODE,
   A1.INGREDIENT,
   A1.STRENGTH || A1.STRENGTH_UNIT || "/" || A1.DOSAGE_VALUE || A1.DOSAGE_UNIT,
   "",
   "SA",
   1
FROM ingred A1, drug A2
WHERE
   A1.DRUG_CODE = A2.DRUG_CODE AND A1.DOSAGE_UNIT != "" AND
   (A1.DOSAGE_VALUE || A1.DOSAGE_UNIT != A1.STRENGTH_UNIT)
   ;


INSERT INTO COMPOSITION
   ("UID", "MOLECULE_CODE", "MOLECULE_NAME", "DOSAGE", "DOSAGE_REF", "NATURE", "LK_NATURE")
SELECT
   A1.DRUG_CODE,
   A1.ACTIVE_INGREDIENT_CODE,
   A1.INGREDIENT,
   A1.STRENGTH || A1.STRENGTH_UNIT,
   "",
   "SA",
   1
FROM ingred A1, drug A2
WHERE
   A1.DRUG_CODE = A2.DRUG_CODE AND (A1.DOSAGE_UNIT = "" OR
   A1.DOSAGE_VALUE || A1.DOSAGE_UNIT = A1.STRENGTH_UNIT)
   ;

-- ********************************
-- Switch DRUG_CODE to DIN in DRUGS and COMPOSITION tables
-- ********************************

UPDATE DRUGS SET UID =
(
  SELECT DIN FROM drug WHERE drug.DRUG_CODE=DRUGS.UID
);

UPDATE COMPOSITION SET UID =
(
  SELECT DIN FROM drug WHERE drug.DRUG_CODE=COMPOSITION.UID
);


-- commented-out until future use
-- ********************************
-- Feed PACKAGING (available shipping sizes / quantities)
-- ********************************
-- Takes account of variability in
-- empty/non-empty fields within the source data

-- INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
-- SELECT A7.DIN, A7.UPC, A7.PRODUCT_INFORMATION, " "
-- FROM package A7
-- WHERE PACKAGE_SIZE = "" AND PRODUCT_INFORMATION != "";

-- INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
-- SELECT A7.DIN, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE, " "
-- FROM package A7
-- WHERE PACKAGE_SIZE != "" AND PRODUCT_INFORMATION = "";

-- INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
-- SELECT A7.DIN, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE || ", " || PRODUCT_INFORMATION, " "
-- FROM package A7
-- WHERE PACKAGE_SIZE != "" AND PRODUCT_INFORMATION != "";

-- INSERT INTO PACKAGING (UID, PACKAGE_UID, LABEL, MARKETING)
-- SELECT A7.DIN, A7.UPC, A7.PACKAGE_SIZE || " " || A7.PACKAGE_SIZE_UNIT || " " || A7.PACKAGE_TYPE || ", " || PRODUCT_INFORMATION, " "
-- FROM package A7
-- WHERE PACKAGE_SIZE = "" AND PRODUCT_INFORMATION = "";

-- the Canadian products mostly lack any unique UPC
-- a pseudo PACKAGE_UID is possible, but might be inconstant
-- UPDATE package SET UPC = ROWID;
