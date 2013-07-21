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
--  * \file be_db_finalize.sql
--  * \author Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.5.4
--  * \date 14 Jan 2011
--  */

-- /**
--  * \brief This script creates the FreeDiams Belguish's drugs database itself.
--  *
--  * \warning SQL commands MUST end with \e ;
--  *
--  */


-- ********************************
-- Feed INFORMATIONS (info about the drug data source)
-- ********************************
UPDATE SOURCES SET
  LANG            = "fr_FR",
  WEB             = "http://www.fagg-afmps.be/",
  COPYRIGHT       = "Public domain",
  DATE            = CURRENT_DATE,
  DRUGS_VERSION   = "0.6.0",
  AUTHORS         = "Eric Maeker, MD (Fr)",
  VERSION         = "0.6.0",
  PROVIDER        = "AFMPS : Agence Fédérale des Médicaments et des Produits de Santé",
  WEBLINK         = "http://www.fagg-afmps.be/fr/items-HOME/Bases_de_donnees/index.jsp",
  DRUG_UID_NAME   = "CIS",
  ATC             = 1,
  INTERACTIONS    = 1,
  COMPLEMENTARY_WEBSITE  = "",
  PACK_MAIN_CODE_NAME    = "CIP",
  DRUGS_NAME_CONSTRUCTOR = "NAME (FORM, STRENGTH)",
  FREEMEDFORMS_COMPTA_VERSION = "0.6.0",
  WEBPORTAL_COMPTA_VERSION    = "0.1.0"
WHERE DATABASE_UID= "FAGG_AFMPS_BE";

VACUUM;
