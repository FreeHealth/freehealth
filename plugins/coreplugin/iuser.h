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
#ifndef CORE_IUSER_H
#define CORE_IUSER_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QVariant>

/**
 * \file iuser.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 17 Feb 2012
*/

namespace Core {

class CORE_EXPORT IUser : public QObject
{
    Q_OBJECT
public:
    enum DataRepresentation {
        // ORDER SHOULD NEVER CHANGE -> SQL Synchronization
        Id = 0,
        Uuid,           /*!< \brief Unique identifier of each user */
        Validity,       /*!< \brief Validity of this user */
        IsVirtual,      /*!< \brief User is virtual?*/
        Login64,        /*!< \brief Encrypted login username \sa UserGlobal::loginForSQL() */
        CryptedPassword,/*!< \brief Encrypted password \sa UserGlobal::crypt() */
        LastLoggedIn,   /*!< \brief Date and Time of the last successful log-in */
        UsualName,      /*!< \brief Lastname */
        OtherNames,     /*!< \brief Second name (name after marriage) */
        Firstname,      /*!< \brief Firstname, ("Given" name) */
        TitleIndex,     /*!< \brief Index of the user title accorging to Trans::ConstantTranslations::titles() */
        GenderIndex,    /*!< \brief Index of the user title accorging to Trans::ConstantTranslations::genders() */
        Mail,           /*!< \brief Mail address */
        LanguageISO,    /*!< \brief Language for interface */
        Locker,         /*!< \brief Locker */
        LocaleLanguage, /*!< \brief Language index from Core::Translators::availableLanguages() */  //12

        // ORDER CAN CHANGE
        DateOfBirth,
        PhotoPixmap,
        ClearLogin,
        ClearPassword,
        LocaleCodedLanguage,
        PersonalLinkId, // Used to link an app object (eg patient) to a user
        FullName,
        Gender,
        Title,
        Street,
        Zipcode,
        City,
        StateProvince,
        Country,
        IsoCountry,
        FullHtmlAddress,
        FullAddress,
        Tel1, // WorkPhone
        Tel2, // LandlinePhone / Home phone
        Tel3, // MobilePhone
        Fax,
        FullHtmlContact,
        Specialities,
        Qualifications,
        ProfessionalIdentifiants,
        Preferences,  // 33

        GenericHeader,
        GenericFooter,
        GenericWatermark,
        GenericHeaderPresence,
        GenericFooterPresence,
        GenericWatermarkPresence,
        GenericWatermarkAlignement,

        AdministrativeHeader,
        AdministrativeFooter,
        AdministrativeWatermark,
        AdministrativeHeaderPresence,
        AdministrativeFooterPresence,
        AdministrativeWatermarkPresence,
        AdministrativeWatermarkAlignement,

        PrescriptionHeader,
        PrescriptionFooter,
        PrescriptionWatermark,
        PrescriptionHeaderPresence,
        PrescriptionFooterPresence,
        PrescriptionWatermarkPresence,
        PrescriptionWatermarkAlignement,

        DataPackConfig,

        ManagerRights,
        DrugsRights,
        MedicalRights,
        ParamedicalRights,
        SecretaryRights,
        AgendaRights,

        DecryptedLogin,           /*!< \brief Clear human readable login. */
        LoginHistory,             /*!< \brief Full login history of the user. */
        IsModified,
        DebugText,                 /*!< \brief For debugging purpose. */
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
    Q_DECLARE_FLAGS(UserRights, UserRight)

    IUser(QObject *parent);
    virtual ~IUser();

    void registerUserTokens() const;

    virtual void clear() = 0;
    virtual bool has(const int ref) const = 0;
    virtual bool hasCurrentUser() const = 0;

    virtual QVariant value(const int ref) const = 0;
    virtual bool setValue(const int ref, const QVariant &value) = 0;

    virtual QString toXml() const = 0;
    virtual bool fromXml(const QString &xml) = 0;

    virtual bool saveChanges() = 0;

    virtual QString fullNameOfUser(const QVariant &uid) = 0;

    void replaceTokens(QString &stringWillBeModified);

    // Some code easiers
    QString uuid() const {return value(Uuid).toString();}
    bool isAdministrator() const;

Q_SIGNALS:
    void userChanged();
    void userDataChanged(const int ref);
    void reset();
};

}  // End Core

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::IUser::UserRights)

#endif // CORE_IUSER_H
