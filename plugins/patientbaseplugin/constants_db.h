/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PATIENTS_DB_CONSTANTS_H
#define PATIENTS_DB_CONSTANTS_H

namespace Patients {
namespace Constants {

    const char * const  DB_NAME            = "patients";
    const char * const  DB_FILENAME        = "patients.db";
    const char * const  DB_ACTUALVERSION   = "0.1";

    // Tables
    enum Tables {
      Table_IDENT = 0,
      Table_PATIENT_PHOTO,
      Table_VERSION
    };

    // Fields
    enum IdentFields {
        IDENTITY_ID = 0,
        IDENTITY_UID,
        IDENTITY_LK_TOPRACT_LKID,
        IDENTITY_FAMILY_UID,
        IDENTITY_ISACTIVE,
        IDENTITY_ISVIRTUAL,
        IDENTITY_NAME,
        IDENTITY_FIRSTNAME,
        IDENTITY_SECONDNAME,
        IDENTITY_GENDER,
        IDENTITY_TITLE,    // Encoded like = 1=Mister, 2=Miss, 3=Madam , 4=Doctor, 5=Professor, 6=Captain
        IDENTITY_DOB,
        IDENTITY_MARITAL_STATUS,
        IDENTITY_DATEOFDEATH,
        IDENTITY_PROFESSION,
        IDENTITY_ADDRESS_STREET,
        IDENTITY_ADDRESS_STREET_NUMBER,
        IDENTITY_ADDRESS_ZIPCODE,
        IDENTITY_ADRESS_CITY,
        IDENTITY_ADDRESS_COUNTRY,
        IDENTITY_ADDRESS_NOTE,
        IDENTITY_MAILS,
        IDENTITY_TELS,
        IDENTITY_FAXES,
        IDENT_VERSION
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
    enum VersionFields {
        VERSION_TEXT = 0
    };

}  // End namespace Patients::Constants
}  // End namespace Patients

#endif // End PATIENTS_DB_CONSTANTS_H
