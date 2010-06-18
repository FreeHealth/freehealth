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
#ifndef IUSER_H
#define IUSER_H

#include <coreplugin/core_exporter.h>

#include <QVariant>

/**
 * \file iuser.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 13 June 2010
*/


namespace Core {

/**
  \brief Use this class to avoid any plugin dependencies (other than Core), when needing to access to patients datas.
*/

class CORE_EXPORT IUser
{
public:
    enum DataRepresentation {
        // ORDER SHOULD NEVER CHANGE
        Id = 0,
        Uuid,           /*!< \brief Unique identifier the each users */
        Validity,       /*!< \brief Validity of this user */
        Login,          /*!< \brief Crypted login \sa UserGlobal::loginForSQL() */
        Password,       /*!< \brief Crypted password \sa UserGlobal::crypt() */
        LastLogin,      /*!< \brief Date of the last correct login */
        Name,           /*!< \brief Name */
        SecondName,     /*!< \brief Second name */
        Surname,        /*!< \brief surname */
        Mail,           /*!< \brief Mail address */
        Language,       /*!< \brief Language for interface */
        Locker,         /*!< \brief Locker */
        LanguageIndex,  /*!< \brief Language index from Core::Translators::availableLanguages() */

        // ORDER CAN CHANGE
        FullName,
        GenderIndex, TitleIndex, Gender, Title,
        Adress, Zipcode, City, Country, Tel1, Tel2, Tel3, Fax, PractitionerId,
        Specialities, Qualifications, Preferences,

        GenericHeader,            /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */
        GenericFooter,            /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */
        GenericWatermark,         /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */

        AdministrativeHeader,     /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */
        AdministrativeFooter,     /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */
        AdministrativeWatermark,  /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */

        PrescriptionHeader,       /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */
        PrescriptionFooter,       /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */
        PrescriptionWatermark,    /*!< \brief Html content of the Extra Document \sa tkTextDocumentExtra */

        ManagerRights,
        DrugsRights,
        MedicalRights,
        ParamedicalRights,
        AdministrativeRights,

        DecryptedLogin,           /*!< \brief Clear human readable login. */
        LoginHistory,             /*!< \brief Full login history of the user. */
        IsModified,
        Warn,                     /*!< \brief For debugging purpose. */
        WarnText,                 /*!< \brief For debugging purpose. */
        NumberOfColumns
    };

    enum UserRight
    {
        NoRights       = 0x00000000,
        ReadOwn        = 0x00000001,
        ReadDelegates  = 0x00000002,
        ReadAll        = 0x00000004,
        WriteOwn       = 0x00000010,
        WriteDelegates = 0x00000020,
        WriteAll       = 0x00000040,
        Print          = 0x00000100,
        Create         = 0x00000200,
        Delete         = 0x00000400,
        AllRights      = ReadOwn | ReadDelegates | ReadAll | WriteOwn | WriteDelegates | WriteAll | Print | Create | Delete
    };
    Q_DECLARE_FLAGS(UserRights, UserRight);

    IUser() {}
    virtual ~IUser() {}

    virtual void clear() = 0;
    virtual bool has(const int ref) const = 0;

    virtual QVariant value(const int ref) const = 0;
    virtual bool setValue(const int ref, const QVariant &value) = 0;

    virtual QString toXml() const = 0;
    virtual bool fromXml(const QString &xml) = 0;

    void replaceTokens(QString &stringWillBeModified);

};

}  // End Core
Q_DECLARE_OPERATORS_FOR_FLAGS(Core::IUser::UserRights)


#endif // IUSER_H
