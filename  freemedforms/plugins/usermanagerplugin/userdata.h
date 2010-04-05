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
 ***************************************************************************/
#ifndef USERDATA_H
#define USERDATA_H

#include <utils/serializer.h>
#include <printerplugin/textdocumentextra.h>

#include <usermanagerplugin/usermanager_exporter.h>
#include <usermanagerplugin/constants.h>
#include <usermanagerplugin/global.h>

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QHash>
class QSqlQuery;

// include Private datas

/**
 * \file userdata.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.16
 * \date 19 Sept 2009
*/

namespace Print {
class TextDocumentExtra;
}

namespace UserPlugin {
namespace Internal {
class UserDataPrivate;
class UserDynamicDataPrivate;


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
        File,              /*!< \brief Value that can be translated to string length over 2000, or is setted as a QFile. */
        Numeric,           /*!< \brief Value that can be translated to integer or double. */
        Date,              /*!< \brief Value that can be translated to date. */
        ExtraDocument      /*!< \brief Value is a tkTextDocumentExtra. */
    };
    UserDynamicData();
    ~UserDynamicData();

    bool isNull() const;
    bool isDirty() const;
    void setDirty(bool state);
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
    QString warnText() const;

//protected:
    void feedFromSql(const int field, const QVariant& value);
//    void feedFromSql(const QHash<int,QVariant> &values);

private:
    UserDynamicDataPrivate *d;
};

using namespace UserPlugin::Constants;

class USER_EXPORT UserData
{
    friend class UserBase;
public:
    UserData();//(QObject * parent = 0);
    //UserData(const UserData&);
    UserData(const QString & uuid);//(QObject * parent = 0);
    ~UserData();

    void setModifiable(bool state);
    void setModified(bool state);
    void setCurrent(bool state);
    bool isCurrent() const;
    bool isModifiable() const;
    bool isModified() const;
    bool isNull() const;
    bool isEmpty() const;
    bool createUuid();
    void setUuid(const QString & val);

    // setters to create or change the values
    void setDynamicDataValue(const char * name, const QVariant & val, UserDynamicData::DynamicDataType t = UserDynamicData::String);
    void setRights(const char * roleName, const User::UserRights rights);


    // simplified setters (TODO : some must be logged)
    void  setId(const QVariant & val)                  { setValue(Table_USERS, USER_ID, val); }
    void  setValidity(const QVariant & val)            { setValue(Table_USERS, USER_VALIDITY, val); }
    void  setLocker(const QVariant & val)               { setValue(Table_USERS, USER_LOCKER ,val); }
    void  setLogin(const QVariant & val)               { setValue(Table_USERS, USER_LOGIN, val); }
    void  setCryptedPassword(const QVariant & val)     { setValue(Table_USERS, USER_PASSWORD, val); }
    void  setLastLogin(const QVariant & val)           { setValue(Table_USERS, USER_LASTLOG, val); }
    void  setTitle(const QVariant & val)               { setDynamicDataValue(USER_DATAS_TITLE, val); }
    void  setGender(const QVariant & val)              { setDynamicDataValue(USER_DATAS_GENDER, val); }
    void  setName(const QVariant & val)                { setValue(Table_USERS, USER_NAME, val); }
    void  setSecondName(const QVariant & val)          { setValue(Table_USERS, USER_SECONDNAME, val); }
    void  setSurname(const QVariant & val)             { setValue(Table_USERS, USER_SURNAME, val); }
    void  setLanguage(const QVariant & val)            { setValue(Table_USERS, USER_LANGUAGE, val); }
    void  setMail(const QVariant & val)                { setValue(Table_USERS, USER_MAIL, val); }

    void  setAdress(const QVariant & val)              { setDynamicDataValue(USER_DATAS_ADRESS , val); }
    void  setZipcode(const QVariant & val)             { setDynamicDataValue(USER_DATAS_ZIPCODE , val); }
    void  setCity(const QVariant & val)                { setDynamicDataValue(USER_DATAS_CITY , val); }
    void  setCountry(const QVariant & val)             { setDynamicDataValue(USER_DATAS_COUNTRY , val); }
    void  setTel1(const QVariant & val)                { setDynamicDataValue(USER_DATAS_TEL1 , val); }
    void  setTel2(const QVariant & val)                { setDynamicDataValue(USER_DATAS_TEL2 , val); }
    void  setTel3(const QVariant & val)                { setDynamicDataValue(USER_DATAS_TEL3 , val); }
    void  setFax(const QVariant & val)                 { setDynamicDataValue(USER_DATAS_FAX , val); }
    void  setPractitionerIdentifiant(const QStringList & val) { setDynamicDataValue(USER_DATAS_PRACTIDENTIFIANT, Utils::Serializer::toString(val)); }

    void  setSpecialty(const QStringList & val)        { setDynamicDataValue(USER_DATAS_SPECIALTY, Utils::Serializer::toString(val)); }
    void  setQualification(const QStringList & val)    { setDynamicDataValue(USER_DATAS_QUALIFICATION, Utils::Serializer::toString(val)); }
    void  setAdminHeader(const QVariant & val)         { setDynamicDataValue(USER_DATAS_ADMINISTRATIVEHEADER , val); }
    void  setAdminFooter(const QVariant & val)         { setDynamicDataValue(USER_DATAS_ADMINISTRATIVEFOOTER , val); }
    void  setPrescriptionHeader(const QVariant & val)  { setDynamicDataValue(USER_DATAS_PRESCRIPTIONHEADER , val); }
    void  setPrescriptionFooter(const QVariant & val)  { setDynamicDataValue(USER_DATAS_PRESCRIPTIONFOOTER , val); }
    void  setPreferences(const QVariant & val)         { setDynamicDataValue(USER_DATAS_PREFERENCES , val); }

    void  addLoginToHistory();

    // simplified getters
    int id() const                       { return value(Table_USERS, USER_ID).toInt(); }
    QString uuid() const                 { return value(Table_USERS, USER_UUID).toString(); }
    bool    validity() const             { return value(Table_USERS, USER_VALIDITY).toBool(); }
    bool    locker() const               { return value(Table_USERS, USER_LOCKER).toBool(); }
    QString login() const                { return value(Table_USERS, USER_LOGIN).toString(); }
    QString decryptedLogin() const       { return UserPlugin::loginFromSQL(value(Table_USERS, USER_LOGIN));  }
    QString cryptedPassword() const      { return value(Table_USERS, USER_PASSWORD).toString(); }
//    QString lastLogin() const;
    QDateTime lastLogin() const          { return value(Table_USERS, USER_LASTLOG).toDateTime(); }
    QString fullName();
    int     title() const                { return dynamicDataValue(USER_DATAS_TITLE).toInt(); }
    int     gender() const               { return dynamicDataValue(USER_DATAS_GENDER).toInt(); }
    QString name() const                 { return value(Table_USERS, USER_NAME).toString(); }
    QString secondName() const           { return value(Table_USERS, USER_SECONDNAME).toString(); }
    QString surname() const              { return value(Table_USERS, USER_SURNAME).toString(); }
    QString language() const             { return value(Table_USERS, USER_LANGUAGE).toString(); }
    QString mail() const                 { return value(Table_USERS, USER_MAIL).toString(); }

    QString adress() const               { return dynamicDataValue(USER_DATAS_ADRESS).toString(); }
    QString zipcode() const              { return dynamicDataValue(USER_DATAS_ZIPCODE).toString(); }
    QString city() const                 { return dynamicDataValue(USER_DATAS_CITY).toString(); }
    QString country() const              { return dynamicDataValue(USER_DATAS_COUNTRY).toString(); }
    QStringList tels() const             { return QStringList()
				      << dynamicDataValue(USER_DATAS_TEL1).toString()
				      << dynamicDataValue(USER_DATAS_TEL2).toString()
				      << dynamicDataValue(USER_DATAS_TEL3).toString();
                                    }
    QString fax() const                  { return dynamicDataValue(USER_DATAS_FAX).toString(); }


    QStringList practitionerId() const       { return Utils::Serializer::toStringList(dynamicDataValue(USER_DATAS_PRACTIDENTIFIANT).toString()); }
    QStringList specialty() const        { return Utils::Serializer::toStringList(dynamicDataValue(USER_DATAS_SPECIALTY).toString());  }
    QStringList qualifications() const       { return Utils::Serializer::toStringList(dynamicDataValue(USER_DATAS_QUALIFICATION).toString()); }

    QStringList adminPapers() const      { return QStringList()
				      << dynamicDataValue(USER_DATAS_ADMINISTRATIVEHEADER).toString()
				      << dynamicDataValue(USER_DATAS_ADMINISTRATIVEFOOTER).toString();
                                    }
    QStringList prescrPapers() const     { return QStringList()
				      << dynamicDataValue(USER_DATAS_PRESCRIPTIONHEADER).toString()
				      << dynamicDataValue(USER_DATAS_PRESCRIPTIONFOOTER).toString();
                                    }

    void setExtraDocument(Print::TextDocumentExtra *extra, const int index);
    void setExtraDocumentHtml(const QVariant &val, const int index);
    QVariant extraDocumentHtml(const int index) const;

    QString preferences() const          { return dynamicDataValue(USER_DATAS_PREFERENCES).toString(); }
    QString loginHistory() const         { return dynamicDataValue(USER_DATAS_LOGINHISTORY).toString(); }

    // Rights getters
    QVariant rightsValue(const char *name) const;

    // viewers
    void warn() const;
    QStringList warnText() const;


protected: // use only with database tkUserBase
    // generic setters to use only when retreiving datas from database
    void setValue(const int tableref, const int fieldref, const QVariant & val);
    void addDynamicDatasFromDatabase(const QList<UserDynamicData*> &list);
    void addRightsFromDatabase(const char * roleName, const int fieldref, const QVariant & val);

    // getters for database
    bool hasModifiedDynamicDatasToStore() const;
    QList<UserDynamicData*> modifiedDynamicDatas() const;
    bool hasModifiedRightsToStore() const;
    QStringList modifiedRoles() const;

    // generic getters for database
    QVariant value(const int tableref, const int fieldref) const;
    QVariant dynamicDataValue(const QString &name, const int fieldref) const;
    QVariant dynamicDataValue(const char *name) const;
    QVariant rightsValue(const QString &name, const int fieldref) const;

private:
    UserDataPrivate *d;
};

}  // End Internal
}  // End namespace UserPlugin

#endif // TKUSER_H
