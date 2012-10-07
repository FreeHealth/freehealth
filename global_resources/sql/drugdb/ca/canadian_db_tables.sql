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
--  * \file canadian_db_tables.sql
--  * \author Jim Busser, MD <jbusser@interchange.ubc.ca>, Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.4.4
--  * \date Dec 12, 2010
--  */

-- /**
--  * \brief This script creates the table structure to hold Canadian downloading
--  *
--  * \warning SQL commands MUST end with \e ; . There MUST BE nothing on the line after the ending \e ; .
--  *
--  * July 14, 2010 initial release
--  *
--  */


-- Create the temporary database schema for importation of the Canadian files
-- for convenience the column name DRUG_IDENTIFICATION_NUMBER is shortened to DIN
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
