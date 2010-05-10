/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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
      Table_LK_TOPRACT,
      Table_EPISODES,
      Table_EPISODE_CONTENT,
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
        IDENTITY_SURNAME,
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
//        LK_TOPRACT_PATIENT_UID,
        LK_TOPRACT_PRACT_UUID,
        LK_TOPRACT_GROUP_UUID
    };
    enum EpisodesFields {
        EPISODES_ID = 0,
        EPISODES_PATIENT_UID,
        EPISODES_LK_TOPRACT_LKID,
        EPISODES_FORM_PAGE_UID,
        EPISODES_LABEL,
        EPISODES_DATE,
        EPISODES_DATEOFCREATION,
        EPISODES_DATEOFMODIFICATION,
        EPISODES_DATEOFVALIDATION,
        EPISODES_VALIDATED
    };
    enum EpisodesContentFields {
        EPISODE_CONTENT_ID = 0,
        EPISODE_CONTENT_EPISODE_ID,
        EPISODE_CONTENT_XML
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
