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
using namespace Core;

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

