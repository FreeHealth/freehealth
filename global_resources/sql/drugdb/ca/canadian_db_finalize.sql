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
--  * NOTE: INSERT INTO "INFORMATIONS" (needs manual updating as to
--  *  i)  the version of the drug db that it is to be known as
--  *  ii) the compatible version of FreeDiams
--  */



-- ********************************
-- Feed INFORMATIONS (info about the drug data source)
-- ********************************
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
    "0.5.2",
    "en=Canadian Drug Product Database
    fr=Base de données thérapeutique Canadienne
    xx=Canadian Drug Product Database
    ",
    "CA_HCDPD",
    "0.5.2",
    "HC: Health Canada Drug Product Database",
    "http://www.hc-sc.gc.ca/dhp-mps/prodpharma/databasdon/index-eng.php",
    "http://code.google.com/p/freemedforms/wiki/Database_ca",
    "Jim Busser, MD (CA) & Eric Maeker, MD (FR)",
    "(c) Her Majesty the Queen in Right of Canada",
    "Apply: http://publications.gc.ca/helpAndInfo/cc-dac/application-e.html on a no-charge, royalty-free, non-exclusive, annual renewable basis.",
    CURRENT_DATE,
    "DIN",
    "UPC",
    1,
    1,
    "While the download is technically open, the data is copyright. Please report any problem to the author.",
    "en_CA",
    "NAME (FORM, GLOBAL_STRENGTH)"
);

-- ***********************************************
-- Localize "Help and Infos" button content to CA
-- ***********************************************
-- localization to province can be achieved by (e.g.) localize-bc_CA

DELETE FROM `SEARCH_ENGINES`;
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Drug’s classification at WHO (query ATC)", "http://www.whocc.no/atc_ddd_index/?&code=[[ONE_ATC_CODE]]&showdescription=yes");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Drug’s status with Health Canada (query DIN)", "http://webprod.hc-sc.gc.ca/dpd-bdpp/index-eng.jsp");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Drug’s status via drugcoverage.ca (manual)", "http://www.drugcoverage.ca/");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "General: NLM Pillbox (Flash pill identifier, beta-test)", "http://pillbox.nlm.nih.gov/bin-release/PillBox.php");


-- ********************************
-- Drop the DPD staging tables
-- ********************************
DROP TABLE drug;
DROP TABLE form;
DROP TABLE route;
DROP TABLE status;
DROP TABLE ingred;
DROP TABLE ther;
DROP TABLE package;

--DROP TABLE form-abbr;

VACUUM;
