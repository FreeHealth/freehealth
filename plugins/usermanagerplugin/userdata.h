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
#ifndef USERPLUGIN_INTERNAL_USERDATA_H
#define USERPLUGIN_INTERNAL_USERDATA_H
#include <usermanagerplugin/usermanager_exporter.h>
#include <usermanagerplugin/constants.h>

#include <coreplugin/iuser.h>

#include <printerplugin/textdocumentextra.h>

#include <utils/global.h>
#include <utils/serializer.h>

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QHash>
#include <QLocale>
#include <QDebug>
QT_BEGIN_NAMESPACE
class QSqlQuery;
QT_END_NAMESPACE

/**
 * \file userdata.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 28 Aug 2013
*/

namespace Print {
class TextDocumentExtra;
}

namespace UserPlugin {
class UserModel;
class UserCreatorWizard;

namespace Internal {
class UserDataPrivate;
class UserDynamicDataPrivate;
#ifdef WITH_TESTS
class UserManagerPlugin;
#endif

class USER_EXPORT UserDynamicData
{
    friend class UserBase;

public:
    /**
       \brief Describe the type of the value.
       \todo Some test need to be done with the type definition.
    */
    enum DynamicDataType {
        String = 0,        /*!< \brief Value that can be translated to string max length = 200. */
        LongString,        /*!< \brief Value that can be translated to string length from 200 to 2000. */
        File,              /*!< \brief Value that can be translated to string length over 2000, or is set as a QFile. */
        Numeric,           /*!< \brief Value that can be translated to integer or double. */
        Date,              /*!< \brief Value that can be translated to date. */
        ExtraDocument      /*!< \brief Value is a tkTextDocumentExtra. */
    };
    UserDynamicData();
    ~UserDynamicData();

    bool isNull() const;
    bool isModified() const;
    void setModified(bool state);

    DynamicDataType type() const;

    QString name() const;
    void setName(const QString &name);
    int id() const;
    void setId(const int id);
    void setUserUuid(const QString &uuid);

    void setValue(Print::TextDocumentExtra *extra);
    void setValue(const QVariant &value);

    Print::TextDocumentExtra *extraDocument() const;
    QVariant value() const;
    void prepareQuery(QSqlQuery &bindedQuery) const;

    void warn() const;
    QString debugText() const;

    bool operator==(const UserDynamicData &other) const;

//protected:
    void feedFromSql(const int field, const QVariant& value);
//    void feedFromSql(const QHash<int,QVariant> &values);

private:
    UserDynamicDataPrivate *d;
};

using namespace UserPlugin::Constants; // TODO remove using directive inside header file

class USER_EXPORT UserData
{
    friend class UserBase;
    friend class UserPlugin::UserModel;
    friend class UserPlugin::UserCreatorWizard;
#ifdef WITH_TESTS
    friend class UserPlugin::Internal::UserManagerPlugin;
#endif

public:
    UserData();
    UserData(const QString & uuid);//(QObject * parent = 0);
    ~UserData();

    void setEditable(bool state);
    void setModified(bool state);
    void setCurrent(bool state);
    void setPasswordModified(bool state);

    bool isCurrent() const;
    bool isEditable() const;
    bool isModified() const;
    bool isPasswordModified() const;
    bool isNull() const;
    bool isEmpty() const;

    bool createUuid();
    void setUuid(const QString &val);

    // setters to create or change the values
    void setDynamicDataValue(const char *name, const QVariant &val, UserDynamicData::DynamicDataType t = UserDynamicData::String);
    void setRights(const char *roleName, const Core::IUser::UserRights rights);


    // simplified setters (TODO : some must be logged)
    void  setId(const QVariant &val)                  { setValue(Table_USERS, USER_ID, val); }
    void  setValidity(bool isvalid)                   { setValue(Table_USERS, USER_VALIDITY, isvalid); }
    void  setVirtual(bool isvirtual)                  { setValue(Table_USERS, USER_ISVIRTUAL, isvirtual); }
    void  setLocker(bool locker)                      { setValue(Table_USERS, USER_LOCKER, locker); }
    void  setClearPassword(const QString &val);
    void  setTitleIndex(int index)                    { setValue(Table_USERS, USER_TITLE, index); }
    void  setGenderIndex(int index)                   { setValue(Table_USERS, USER_GENDER, index); }
    void  setUsualName(const QVariant &val)            { setValue(Table_USERS, USER_USUALNAME, val); }
    void  setOtherNames(const QVariant &val)          { setValue(Table_USERS, USER_OTHERNAMES, val); }
    void  setFirstname(const QVariant &val)           { setValue(Table_USERS, USER_FIRSTNAME, val); }
    void  setLanguageIso(const QVariant &val)          { setValue(Table_USERS, USER_LANGUAGE, val); }
    void  setLocaleLanguage(QLocale::Language lang)    { setValue(Table_USERS, USER_LANGUAGE, QLocale(lang).name().left(2)); }
    void  setMail(const QVariant &val)                { setValue(Table_USERS, USER_MAIL, val); }

    void  setPhoto(const QPixmap &pix);

    void  setDob(const QVariant &val)                { setDynamicDataValue(USER_DATA_DOB, val); }
    void  setStreet(const QVariant &val)             { setDynamicDataValue(USER_DATA_STREET, val); }
    void  setZipcode(const QVariant &val)             { setDynamicDataValue(USER_DATA_ZIPCODE , val); }
    void  setStateProvince(const QVariant &val)       { setDynamicDataValue(USER_DATA_STATEPROVINCE , val); }
    void  setCity(const QVariant &val)                { setDynamicDataValue(USER_DATA_CITY , val); }
    void  setCountry(const QVariant &val)             { setDynamicDataValue(USER_DATA_COUNTRY , val); }
    void  setCountryIso(const QVariant &val)          { setDynamicDataValue(USER_DATA_COUNTRY , val); }
    void  setTel1(const QVariant &val)                { setDynamicDataValue(USER_DATA_TEL1 , val); }
    void  setTel2(const QVariant &val)                { setDynamicDataValue(USER_DATA_TEL2 , val); }
    void  setTel3(const QVariant &val)                { setDynamicDataValue(USER_DATA_TEL3 , val); }
    void  setFax(const QVariant &val)                 { setDynamicDataValue(USER_DATA_FAX , val); }
    void  setPractitionerIdentifiant(const QStringList &val) { setDynamicDataValue(USER_DATA_PRACTIDENTIFIANT, Utils::Serializer::toString(val)); }

    void  setSpecialty(const QStringList &val)         { setDynamicDataValue(USER_DATA_SPECIALTY, Utils::Serializer::toString(val)); }
    void  setQualification(const QStringList &val)     { setDynamicDataValue(USER_DATA_QUALIFICATION, Utils::Serializer::toString(val)); }

    void  setGenericHeader(const QVariant &val)        { setDynamicDataValue(USER_DATA_GENERICHEADER , val); }
    void  setGenericFooter(const QVariant &val)        { setDynamicDataValue(USER_DATA_GENERICFOOTER , val); }
    void  setGenericWatermark(const QVariant &val)      { setDynamicDataValue(USER_DATA_GENERICWATERMARK , val); }

    void  setAdminHeader(const QVariant &val)          { setDynamicDataValue(USER_DATA_ADMINISTRATIVEHEADER , val); }
    void  setAdminFooter(const QVariant &val)          { setDynamicDataValue(USER_DATA_ADMINISTRATIVEFOOTER , val); }
    void  setAdminWatermark(const QVariant &val)        { setDynamicDataValue(USER_DATA_ADMINISTRATIVEWATERMARK , val); }

    void  setPrescriptionHeader(const QVariant &val)   { setDynamicDataValue(USER_DATA_PRESCRIPTIONHEADER , val); }
    void  setPrescriptionFooter(const QVariant &val)   { setDynamicDataValue(USER_DATA_PRESCRIPTIONFOOTER , val); }
    void  setPrescriptionWatermark(const QVariant &val) { setDynamicDataValue(USER_DATA_PRESCRIPTIONWATERMARK , val); }

    void  setPreferences(const QVariant &val)          { setDynamicDataValue(USER_DATA_PREFERENCES , val); }

    // function needs to be rewritten see issue #61
    //void  addLoginToHistory();

    // simplified getters
    int id() const                       { return value(Table_USERS, USER_ID).toInt(); }
    QString uuid() const                 { return value(Table_USERS, USER_UUID).toString(); }
    bool    validity() const             { return value(Table_USERS, USER_VALIDITY).toBool(); }
    bool isVirtual() const               { return value(Table_USERS, USER_ISVIRTUAL).toBool(); }
    bool    locker() const               { return value(Table_USERS, USER_LOCKER).toBool(); }
    QString login64() const              { return value(Table_USERS, USER_LOGIN).toString(); }
    QString clearLogin() const           { return QString(QByteArray::fromBase64(login64().toUtf8())); }
    QString decryptedLogin() const;

    /** Returns the user password extracted from the database */
    QString clearPassword() const;
    QString cryptedPassword() const      { return value(Table_USERS, USER_PASSWORD).toString(); }

    QDateTime lastLoggedIn() const          { return value(Table_USERS, USER_LASTLOG).toDateTime(); }
    QString fullName() const;
    QString title() const;
    int     titleIndex() const           { return value(Table_USERS, USER_TITLE).toInt(); }
    QString gender() const;
    int     genderIndex() const          { return value(Table_USERS, USER_GENDER).toInt(); }
    QString usualName() const                 { return value(Table_USERS, USER_USUALNAME).toString(); }
    QString otherNames() const           { return value(Table_USERS, USER_OTHERNAMES).toString(); }
    QString firstname() const            { return value(Table_USERS, USER_FIRSTNAME).toString(); }
    QString languageIso() const          { return value(Table_USERS, USER_LANGUAGE).toString(); }
    QLocale::Language localeLanguage() const {return QLocale(languageIso()).language(); }
    QString mail() const                 { return value(Table_USERS, USER_MAIL).toString(); }

    QPixmap photo() const;

    QDate dob() const                    { return dynamicDataValue(USER_DATA_DOB).toDate(); }
    QString street() const               { return dynamicDataValue(USER_DATA_STREET).toString(); }
    QString zipcode() const              { return dynamicDataValue(USER_DATA_ZIPCODE).toString(); }
    QString stateProvince() const        { return dynamicDataValue(USER_DATA_STATEPROVINCE).toString(); }
    QString city() const                 { return dynamicDataValue(USER_DATA_CITY).toString(); }
    QString country() const              { return dynamicDataValue(USER_DATA_COUNTRY).toString(); }
    QString countryIso() const           { return dynamicDataValue(USER_DATA_COUNTRY).toString(); }
    QStringList tels() const             { return QStringList()
                                           << dynamicDataValue(USER_DATA_TEL1).toString()
                                           << dynamicDataValue(USER_DATA_TEL2).toString()
                                           << dynamicDataValue(USER_DATA_TEL3).toString(); }
    QString fax() const                  { return dynamicDataValue(USER_DATA_FAX).toString(); }


    QStringList professionalIdentifiants() const   { return Utils::Serializer::toStringList(dynamicDataValue(USER_DATA_PRACTIDENTIFIANT).toString()); }
    QStringList specialties() const        { return Utils::Serializer::toStringList(dynamicDataValue(USER_DATA_SPECIALTY).toString());  }
    QStringList qualifications() const   { return Utils::Serializer::toStringList(dynamicDataValue(USER_DATA_QUALIFICATION).toString()); }

    QStringList adminPapers() const      { return QStringList()
                                           << dynamicDataValue(USER_DATA_ADMINISTRATIVEHEADER).toString()
                                           << dynamicDataValue(USER_DATA_ADMINISTRATIVEFOOTER).toString(); }
    QStringList prescrPapers() const     { return QStringList()
                                           << dynamicDataValue(USER_DATA_PRESCRIPTIONHEADER).toString()
                                           << dynamicDataValue(USER_DATA_PRESCRIPTIONFOOTER).toString();}

    void setExtraDocument(Print::TextDocumentExtra *extra, const int index);
    void setExtraDocumentHtml(const QVariant &val, const int index);
    void setExtraDocumentPresence(const int presence, const int index);

    QVariant extraDocumentHtml(const int index) const;
    Print::TextDocumentExtra *extraDocument(const int index) const;

    QString preferences() const          { return dynamicDataValue(USER_DATA_PREFERENCES).toString(); }
    QString loginHistory() const         { return dynamicDataValue(USER_DATA_LOGINHISTORY).toString(); }

    // Rights getters
    QVariant rightsValue(const char *name) const;
    bool hasRight(const char *name, const int rightToTest) const;

    // viewers
    void warn() const;
    QString debugText() const;

protected:
    // The following should only be used in UserBase / UserCreatorWizard
    // generic setters to use only when retrieving data from database
    void setValue(const int tableref, const int fieldref, const QVariant &val);
    void addDynamicDataFromDatabase(const QList<UserDynamicData*> &list);
    void addRightsFromDatabase(const char * roleName, const int fieldref, const QVariant &val);

    // Login can not be changed for any users
    void setLogin64(const QVariant &val)             { setValue(Table_USERS, USER_LOGIN, val); }
    void setLastLoggedIn(const QVariant &val)           { setValue(Table_USERS, USER_LASTLOG, val); }
    // Crypted password must always be sync with clear password
    void setCryptedPassword(const QVariant &val);

    // getters for database
    bool hasModifiedDynamicDataToStore() const;
    bool hasModifiedRightsToStore() const;
    QList<UserDynamicData*> modifiedDynamicData() const;
    QStringList modifiedRoles() const;

    // generic getters for database
    QVariant value(const int tableref, const int fieldref) const;
    QVariant dynamicDataValue(const QString &name, const int fieldref) const;
    QVariant dynamicDataValue(const char *name) const;
    QVariant rightsValue(const QString &name, const int fieldref) const;

private:
    UserDataPrivate *d;
};

}  // namespace Internal
}  // namespace UserPlugin

USER_EXPORT QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserData &c);
USER_EXPORT QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserData *c);
USER_EXPORT QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserDynamicData &c);
USER_EXPORT QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserDynamicData *c);

#endif // USERPLUGIN_INTERNAL_USERDATA_H
