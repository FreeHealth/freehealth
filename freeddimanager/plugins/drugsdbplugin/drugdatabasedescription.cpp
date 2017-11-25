/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DrugsDB::DrugDatabaseDescription
 * \n FreeToolBox specific class.
 */

#include "drugdatabasedescription.h"

//<DrugDbDescriptor>

//  <description>
//    <lang>fr_FR</lang>
//    <version>0.6.4<version>
//    <authors>Eric Maeker, MD (Fr)</authors>
//    <fmfv>0.6.2</fmfv>
//    <fdv>0.6.4</fdv>
//    <vendor>AFSSAPS</vendor>
//    <cdate>2011-11-26</cdate>
//    <lmdate>2011-12-12</lmdate>
//    <weblink>http://afssaps-prd.afssaps.fr/php/ecodex/index.php</weblink>
//    <copyright>Public domain</copyright>
//    <druguidbname>CIS</druguidname>
//    <atcvalid>true</atcvalid>
//    <interactionvalid>true</interactionvalid>
//    <complementweblink></complementweblink>
//    <packuidname>CIP</packuidname>
//    <drugnameconstructor></drugnameconstructor>
//    <uuid>FR_AFSSAPS</uuid>
//  </description>

//</DrugDbDescriptor>
using namespace DrugsDb;

namespace {
const char * const TAG_ROOT = "DrugDbDescriptor";
const char * const TAG_DESCRIPTION = "description";
}

DrugDatabaseDescription::DrugDatabaseDescription() :
    Utils::GenericDescription(TAG_DESCRIPTION)
{
    addNonTranslatableExtraData(Language, "lang");
    addNonTranslatableExtraData(Vendor, "vendor");
    addNonTranslatableExtraData(Copyright, "copyright");
    addNonTranslatableExtraData(DrugUidName, "druguidbname");
    addNonTranslatableExtraData(IsAtcValid, "atcvalid");
    addNonTranslatableExtraData(IsDDIValid, "interactionvalid");
    addNonTranslatableExtraData(ComplementaryWebLink, "complementweblink");
    addNonTranslatableExtraData(PackUidName, "packuidname");
    addNonTranslatableExtraData(DrugNameConstructor, "drugnameconstructor");

}

