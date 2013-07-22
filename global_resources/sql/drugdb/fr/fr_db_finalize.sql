-- /***************************************************************************
--  *  The FreeMedForms project is a set of free, open source medical         *
--  *  applications.                                                          *
--  *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
--  *  All rights reserved.                                                   *
--  *                                                                         *
--  *  This program is free software: you can redistribute it and/or modify   *
--  *  it under the terms of the GNU General Public License as published by   *
--  *  the Free Software Foundation, either version 3 of the License, or      *
--  *  (at your option) any later version.                                    *
--  *                                                                         *
--  *  This program is distributed in the hope that it will be useful,        *
--  *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
--  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
--  *  GNU General Public License for more details.                           *
--  *                                                                         *
--  *  You should have received a copy of the GNU General Public License      *
--  *  along with this program (COPYING.FREEMEDFORMS file).                   *
--  *  If not, see <http://www.gnu.org/licenses/>.                            *
--  ***************************************************************************/

-- /**
--  * \file fr_db_finalize.sql
--  * \author Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.5.4
--  * \date 12 Jan 2011
--  */

-- /**
--  * \brief This script creates the FreeDiams Belguish's drugs database itself.
--  *
--  * \warning SQL commands MUST end with \e ;
--  *
--  * NOTE: INSERT INTO "INFORMATIONS" (needs manual updating as to
--  *  i)  the version of the drug db that it is to be known as
--  *  ii) the compatible version of FreeDiams
--  */

-- Manage missing molecule_names
-- select distinct molecule_code from TMP_COMPOSITION where molecule_name is null;
-- --> 1674 7055 7239 79482 87268
-- select molecule_name from TMP_COMPOSITION where (molecule_code=7055 and molecule_name is not null);

-- ********************************
-- Feed INFORMATIONS (info about the drug data source)
-- ********************************
UPDATE SOURCES SET
  LANG            = "fr_FR",
  WEB             ="http://www.afssaps.fr/",
  COPYRIGHT       ="Public domain",
  DATE            =CURRENT_DATE,
  DRUGS_VERSION   ="0.6.0",
  AUTHORS         ="Eric Maeker, MD (Fr)",
  VERSION         ="0.6.0",
  PROVIDER        ="AFSSAPS",
  WEBLINK         ="http://afssaps-prd.afssaps.fr/php/ecodex/index.php",
  DRUG_UID_NAME   ="CIS",
  ATC             =1,
  INTERACTIONS    =1,
  COMPLEMENTARY_WEBSITE  ="",
  PACK_MAIN_CODE_NAME    ="CIP",
  DRUGS_NAME_CONSTRUCTOR ="",
  FREEMEDFORMS_COMPTA_VERSION ="0.6.0",
  WEBPORTAL_COMPTA_VERSION    ="0.1.0"
WHERE DATABASE_UID="FR_AFSSAPS";

VACUUM;
