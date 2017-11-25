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
#ifndef USERCONSTANTS_H
#define USERCONSTANTS_H

#include <qglobal.h>

/**
 \brief all constants used by the usertoolkit objects and widgets.
 \ingroup usertoolkit constants_usertoolkit
  \ingroup usermanager
*/
namespace UserPlugin {
namespace Constants {

    // User Database connection
    const char * const DB_NAME           = "users";
    const char * const USER_DB_VERSION              = "0.9.8";
    const int          DB_CURRENT_CODE_VERSION      = 1;

    // constants for DATA table represent DATANAME field
    const char * const USER_DATA_PHOTO             = "identity.photo";
    const char * const USER_DATA_DOB               = "identity.dob";
    const char * const USER_DATA_STREET            = "identity.adress";
    const char * const USER_DATA_ZIPCODE           = "identity.zip";
    const char * const USER_DATA_STATEPROVINCE     = "identity.state";
    const char * const USER_DATA_CITY              = "identity.city";
    const char * const USER_DATA_COUNTRY           = "identity.country";
    const char * const USER_DATA_TEL1              = "identity.tel1";
    const char * const USER_DATA_TEL2              = "identity.tel2";
    const char * const USER_DATA_TEL3              = "identity.tel3";
    const char * const USER_DATA_FAX               = "identity.fax";
    const char * const USER_DATA_PRACTIDENTIFIANT  = "identity.identifiant";
    const char * const USER_DATA_SPECIALTY         = "identity.specialty";
    const char * const USER_DATA_QUALIFICATION     = "identity.qualification";
    const char * const USER_DATA_PREFERENCES       = "pref";
    const char * const USER_DATA_DATAPACK_CONFIG   = "config.datapack";
    const char * const USER_DATA_LOGINHISTORY      = "loginHistory";


    const char* const  PAPERS_MARK                     = "papers";                  /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_GENERICHEADER        = "papers.generic.header";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_GENERICFOOTER        = "papers.generic.footer";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_GENERICWATERMARK     = "papers.generic.watermark";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_ADMINISTRATIVEHEADER = "papers.admin.header";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_ADMINISTRATIVEFOOTER = "papers.admin.footer";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_ADMINISTRATIVEWATERMARK = "papers.admin.watermark";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_PRESCRIPTIONHEADER   = "papers.prescription.header";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_PRESCRIPTIONFOOTER   = "papers.prescription.footer";   /** \brief x \sa tkUser::setDynamicDataValue() */
    const char * const USER_DATA_PRESCRIPTIONWATERMARK= "papers.prescription.watermark";   /** \brief x \sa tkUser::setDynamicDataValue() */

//    const char * const USER_ROLE_SUPERADMIN      = "role.superadmin";
    // if you add or delete a role, you must change UserRightsWidget
    const char * const USER_ROLE_USERMANAGER     = "role.userManager";
    const char * const USER_ROLE_MEDICAL         = "role.medical";
    const char * const USER_ROLE_DOSAGES         = "role.dosage";
    const char * const USER_ROLE_PARAMEDICAL     = "role.paramedical";
    const char * const USER_ROLE_SECRETARY       = "role.secretary";
    const char * const USER_ROLE_AGENDA          = "role.agenda";

    const char * const DEFAULT_USER_UUID      = "{bff46aaa-5fed-481a-8af3-d5985c549e9c}";
    const char * const DEFAULT_USER_LOGIN     = "Zm1mX2FkbWlu";                  // fmf_admin
    const char * const DEFAULT_USER_PASSWORD  = "TRWR6Wtzaiou0phnGmGphgO2MTg=";  // fmf_admin
    const char * const DEFAULT_USER_CLEARLOGIN     = "fmf_admin";
    const char * const DEFAULT_USER_CLEARPASSWORD  = "fmf_admin";
    const char * const DEFAULT_USER_NAME      = "ADMINISTRATOR";
    const char * const DEFAULT_USER_FIRSTNAME = "Admin";
    const char * const DEFAULT_USER_ADDRESS   = "You should change the login/password of this user.";

    const char * const DEFAULT_USER_ROLE      = USER_ROLE_USERMANAGER;
    int const          DEFAULT_USER_RIGHT     = -1;
    const char * const DEFAULT_USER_SPECIALTY = "Default Super Administrator";
    const char * const DEFAULT_USER_HEADER    =
            "<html><body>" \
            "<table width=100% border=2>" \
            "<tr>" \
            " <td>This is simple sample of what</td>" \
            " <td>&nbsp;</td>" \
            " <td>you can do with this software</td>" \
            "</tr>" \
            "<tr>" \
            " <td>[:identity.title] [:identity.fullname] adds your recorded title then your name</td>" \
            " <td>&nbsp;</td>" \
            " <td>&nbsp;</td>" \
            "</tr>" \
            "<tr>" \
            " <td>[:identity.identifiant] adds your recorded ids</td>" \
            " <td>&nbsp;</td>" \
            " <td>&nbsp;</td>" \
            "</tr>" \
            "<tr>" \
            " <td>[:identity.specialty] adds your recorded specialties<br>" \
            "     [:identity.qualification] adds your recorded qualifications</td>" \
            " <td>&nbsp;</td>" \
            " <td>&nbsp;</td>" \
            "</tr>" \
            "</table>"  \
            "</body></html>";
    const char * const DEFAULT_USER_FOOTER   =
            "<html><body>"  \
            "Here you can add emergency telephone numbers<br>"  \
            "Or What ever you want."  \
            "</body></html>";


    // For database management
    /** \brief Enumerates all the tables of the users' database */
    enum Tables
    {
        Table_USERS=0,
        Table_DATA,
        Table_RIGHTS,
        Table_GROUPS,
        Table_SCHEMA,
        Table_MaxParam
    };

    /** \brief Enumerates the fields of the table USERS */
    enum USER_FIELDS
    {
        USER_ID = 0,
        USER_UUID,
        USER_VALIDITY,
        USER_ISVIRTUAL,
        USER_LOGIN,
        USER_PASSWORD,
        USER_LASTLOG,
        USER_USUALNAME,
        USER_OTHERNAMES,
        USER_FIRSTNAME,
        USER_TITLE,
        USER_GENDER,
        USER_MAIL,
        USER_LANGUAGE,
        USER_LOCKER,
        USER_MaxParam
    };

    /** \brief Enumerates the fields of the table USERS_DATA */
    enum DATAfields
    {
        DATAS_ID = 0 , DATA_USER_UUID,
        DATA_DATANAME, DATA_STRING, DATA_LONGSTRING, DATA_FILE, DATA_NUMERIC, DATA_DATE,
        DATA_LANGUAGE, DATA_LASTCHANGE, DATA_TRACE_ID,
        DATA_MaxParam
    };

    /** \brief Enumerates the fields of the table USERS_RIGHTS */
    enum RIGHTSfields  // 1 USER --> N RIGHTS defined by role
    {
        // Rights
        RIGHTS_ID=0,
        RIGHTS_USER_UUID,
        RIGHTS_ROLE, RIGHTS_RIGHTS,
        RIGHTS_MaxParam
    };

    enum GROUPSfields
    {
        GROUPS_ID = 0,
        GROUPS_UID,
        GROUPS_USER_UID,
        GROUPS_PARENT_GROUP_UID
    };

    enum SchemaFields {
        SCHEMA_ID =0 ,
        SCHEMA_VERSION,
        SCHEMA_SCRIPT,
        SCHEMA_TIMESTAMP
    };

    // Translations
    const char* const TR_CONTEXT_USERS = "UserPlugin";

    const char* const USER_WIZARD = QT_TRANSLATE_NOOP("UserPlugin", "User creator wizard");
    const char* const CREATE_USER = QT_TRANSLATE_NOOP("UserPlugin", "Create user");
    const char* const MODIFY_USER = QT_TRANSLATE_NOOP("UserPlugin", "Modify user");
    const char* const DELETE_USER = QT_TRANSLATE_NOOP("UserPlugin", "Delete user");
    const char* const SAVE_USER = QT_TRANSLATE_NOOP("UserPlugin", "Save user");


}  // End Constants
}  // End UserPlugin

#endif // USERCONSTANTS_H
