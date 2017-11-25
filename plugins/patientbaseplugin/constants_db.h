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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_DB_CONSTANTS_H
#define PATIENTS_DB_CONSTANTS_H

#include <QColor>

namespace Patients {
namespace Constants {

    const char * const DB_NAME            = "patients";
    const char * const DB_FILENAME        = "patients.db";
    const char * const DB_INITIAL_VERSION = "0.1";
    const char * const OLD_VERSION_TABLE  = "VERSION";
    const char * const OLD_VERSION_FIELD  = "VERSION";
    //bump for i96 const int          DB_CURRENT_CODE_VERSION  = 1;
    const int          DB_CURRENT_CODE_VERSION  = 2;

    // Tables
    enum Tables {
      Table_IDENT = 0,
      Table_PATIENT_PHOTO,
      //Table_VERSION
      Table_SCHEMA
    };

    // Fields
    enum IdentFields {
        IDENTITY_ID = 0,
        IDENTITY_UID,
        IDENTITY_LK_TOPRACT_LKID, // Not used. Will be removed in a future release.
        IDENTITY_FAMILY_UID,
        IDENTITY_ISACTIVE,
        IDENTITY_ISVIRTUAL,
        IDENTITY_USUALNAME,
        IDENTITY_FIRSTNAME,
        IDENTITY_OTHERNAMES,
        IDENTITY_GENDER,
        IDENTITY_TITLE,    // Encoded using the Utils::titles() QStringList index
        IDENTITY_DOB,
        IDENTITY_MARITAL_STATUS,
        IDENTITY_DATEOFDEATH,
        IDENTITY_PROFESSION,
        IDENTITY_ADDRESS_STREET,
        IDENTITY_ADDRESS_STREET_NUMBER,
        IDENTITY_ADDRESS_ZIPCODE,
        IDENTITY_ADDRESS_CITY,
        IDENTITY_ADDRESS_PROVINCE,
        IDENTITY_ADDRESS_COUNTRY,
        IDENTITY_ADDRESS_NOTE,
        IDENTITY_MAILS,
        IDENTITY_TELS,
        IDENTITY_FAXES,
        IDENTITY_MOBILE_PHONE,
        IDENTITY_WORK_PHONE,
        IDENTITY_USER_UUID
    };
    enum LkFields {
        LK_TOPRACT_ID = 0,
        LK_TOPRACT_LKID,
        LK_TOPRACT_PRACT_UUID,
        LK_TOPRACT_GROUP_UUID
    };
    enum PhotoFields {
        PHOTO_ID = 0 ,
        PHOTO_UID,
        PHOTO_PATIENT_UID,
        PHOTO_BLOB
    };
    /*enum VersionFields {
        VERSION_TEXT = 0
    };*/
    enum SchemaFields {
        SCHEMA_ID =0 ,
        SCHEMA_VERSION,
        SCHEMA_SCRIPT,
        SCHEMA_TIMESTAMP
    };

    static const QColor femaleColor(255, 207, 255, 90);  //ffcfff
    static const QColor maleColor(225, 225, 255, 90);    //e0e0ff
    static const QColor hermaColor(255, 214, 255, 90);   //ffd6ff

}  // End namespace Patients::Constants
}  // End namespace Patients

#endif // End PATIENTS_DB_CONSTANTS_H
