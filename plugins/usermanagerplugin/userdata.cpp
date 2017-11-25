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
/**
  \class UserPlugin::UserDynamicData
  \brief Stores dynamic data of users.
  Dynamic data can be :
  \li strings
  \li datetime value
  \li files and images (coded into database into base64)
  \li integer or double
  \li a Print::TextDocumentExtra pointer or html contents
  Get information of the database structure in UserBase. \n
  Unit-tests are available.
  \sa UserPlugin::Internal::UserBase, UserPlugin::Internal::UserManagerPlugin
*/

/**
  \class UserPlugin::UserData
  \brief This class owns the user data.
  This class is a link between UserBase and UserModel. You should never use it directly to access user's data.\n
  All the user's data are available via the UserModel.

  Some members are reserved for the use with UserBase and shouldn't be assessed outside of the UserBase class.\n
  You can set/get values using database tables representations with :
  - setValue() and value() for the USERS table,
  - addDynamicDataFromDatabase() and dynamicDataValue() for the DATA table,
  - addRightsFromDatabase() and rightsValue() for the RIGHTS table,
  - hasModifiedDynamicDataToStore(), hasModifiedRightsToStore inform UserBase of needed changes,
  - modifiedDynamicData(), modifiedRoles() inform UserBase of the dynamic data and rights to save to base.

  Some members are reserved for users interactions. Theses members are mainly assessed by the UserModel.
  - setDynamicData() can be used for creating a new dynamic data or change the value of the dynamic data,
  - setRights() can be used for creating new rights or change the value of the rights.
  - You can set/get unique value using simplified setters and simplified getters. Ex : setId(), id()...
  - A editable state can be set/get using seteditable() and isEditable(). If the user is not editable,
  you can not set values. The isNull() value is set in the constructor, and change at the first data modification.

  Unit-tests are available.
  \sa UserPlugin::Internal::UserData, UserPlugin::Internal::UserBase, UserPlugin::Internal::UserManagerPlugin
*/

#include "userdata.h"
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/serializer.h>
#include <utils/passwordandlogin.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <printerplugin/textdocumentextra.h>

#include <QApplication>
#include <QDateTime>
#include <QHash>
#include <QVariant>
#include <QSet>

using namespace UserPlugin;
using namespace Internal;
using namespace UserPlugin::Constants;
using namespace Trans::ConstantTranslations;

static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

namespace UserPlugin {
namespace Internal {
class UserDynamicDataPrivate {
public:
    UserDynamicDataPrivate() :
        m_IsDirty(false),
        m_IsNull(true),
        m_Id(-1),
        m_Trace(-1),
        m_Type(UserDynamicData::String),
        m_Doc(0)
    {
        m_Language = QLocale().name().left(2);
    }

    ~UserDynamicDataPrivate() {}

    // Defines the Html content of the extraDocument
    void setDocumentHtml(const QVariant &value)
    {
        if (!m_Doc)
            m_Doc = new Print::TextDocumentExtra();
        m_Doc->setHtml(value.toString());
    }

    // Defines the Xml of the extraDocument
    void setDocumentXml(const QVariant &value)
    {
        if (!m_Doc)
            m_Doc = new Print::TextDocumentExtra();
        m_Doc = Print::TextDocumentExtra::fromXml(value.toString());
    }

    // Set this object as dirty (modified)
    void setDirty()
    {
        m_IsNull = false;
        m_IsDirty = true;
        m_Lastchange = QDateTime::currentDateTime();
    }


public:
    QString m_Name;       /*!< \brief Name of the UserDynamicData. */
    bool m_IsDirty;       /*!< \brief Use to mark modified UserDynamicData to save into database. */
    bool m_IsNull;        /*!< \brief */
    int m_Id;             /*!< \brief Id of the UserDynamicData*/
    int m_Trace;          /*!< \brief Link to the 'trace' table of the data */
    QString m_UserUuid;   /*!< \brief User Uuid of the UserDynamicData*/
    QVariant m_Value;     /*!< \brief Value stored into a QVariant. Set to null for tkTextDocumentExtra. */
    QString m_Language;   /*!< \brief Language compatible */
    QDateTime m_Lastchange;                     /*!< \brief Date of the last change. */
    UserDynamicData::DynamicDataType m_Type;    /*!< \brief Type of the UserDynamicData. \sa */
    Print::TextDocumentExtra *m_Doc;                 /*!< \brief Defined if the UserDynamicData is a tkTextDocumentExtra. Pointer is deleted in the destructor. */
};

}  // End Internal
}  // End namespace UserPlugin


UserDynamicData::UserDynamicData() :
    d(0)
{
    d = new UserDynamicDataPrivate();
}

UserDynamicData::~UserDynamicData()
{
    if (d)
        delete d;
    d=0;
}

/** \brief Returns true if the data is actually null */
bool UserDynamicData::isNull() const
{
    return d->m_IsNull;
}

/** \brief Returns true if the value has change since last call of feedFromSql(). */
bool UserDynamicData::isModified() const
{
    return d->m_IsDirty;
}

/** Define the modification state of the UserDynamicData. */
void UserDynamicData::setModified(bool state)
{
    d->m_IsDirty = state;
}

/**
 * Returns the type of data. This type is automatically defined
 * when a value is set.
 * \sa setValue();
 */
UserDynamicData::DynamicDataType UserDynamicData::type() const
{
    return d->m_Type;
}

/** \brief Returns the name of the UserDynamicData */
QString UserDynamicData::name() const
{
    return d->m_Name;
}

/** \brief Defines the name of UserDynamicData */
void UserDynamicData::setName(const QString &name)
{
    d->m_Name = name;
    if (d->m_Name.startsWith(PAPERS_MARK)) {
        d->m_Type = ExtraDocument;
    }
    d->setDirty();
}

/** \brief Returns the Id of the UserDynamicData */
int UserDynamicData::id() const
{
    return d->m_Id;
}

/** \brief Defines the id. Used by UserBase::saveUser(), when UserDynamicData is created into database. */
void UserDynamicData::setId(const int id)
{
    d->m_Id = id;
    d->setDirty();
}

/** \brief Defines the related user's uuid of UserDynamicData */
void UserDynamicData::setUserUuid(const QString &uuid)
{
    d->m_UserUuid = uuid;
    d->setDirty();
}

/**
  \brief Feed the class with data without marking it as dirty. Only used by UserBase.
  TextDocumentExtra values should be set from complete xml encoded TextDocumentExtra.
*/
void UserDynamicData::feedFromSql(const int field, const QVariant& value)
{
    Q_ASSERT(field>=DATAS_ID && field <= DATA_TRACE_ID);
    switch (field) {
    case DATAS_ID : d->m_Id = value.toInt(); break;
    case DATA_USER_UUID : d->m_UserUuid = value.toString(); break;
    case DATA_DATANAME: setName(value.toString()); break;
    case DATA_LANGUAGE: d->m_Language = value.toString(); break;
    case DATA_LASTCHANGE: d->m_Lastchange = value.toDateTime(); break;
    case DATA_TRACE_ID: d->m_Trace = value.toInt(); break;
    default: // Store the value
    {
        if (value.isNull())
            break;
        if (value.toString().isEmpty())
            break;
        if (d->m_Type==ExtraDocument)
            d->setDocumentXml(value);
        else {
            if (value.type() == QVariant::DateTime)
                d->m_Type = Date;
            else if (value.type() == QVariant::String)
                d->m_Type = String;
            d->m_Value = value;
        }
        break;
    }
    }
    d->m_IsNull = false;
}

/** \brief Defines this dynamic data as a Print::TextDocumentExtra. */
void UserDynamicData::setValue(Print::TextDocumentExtra *extra)
{
    if (!extra)
        return;
    d->m_Type = ExtraDocument;
    if (d->m_Doc)
        delete d->m_Doc;
    d->m_Doc = extra;
    d->m_Value = QVariant();
    setModified(true);
}

/**
 * Defines the values of the dynamic data. If it is a Print::TextDocumentExtra
 * defines the html code.
 */
void UserDynamicData::setValue(const QVariant &value)
{
    if (d->m_Type==ExtraDocument) {
        d->setDocumentHtml(value);
        d->setDirty();
   } else if (d->m_Value != value) {
        d->m_Value = value;
        d->setDirty();
    }
}

/**
  \brief Return the Print::TextDocumentExtra related to this user dynamic data 
  or 0 if this data does not correspond to a tkTextDocuementExtra.
  The returned pointer SHOULD NOT BE deleted !!!
  \sa UserDynamicData::DynamicDataType
*/
Print::TextDocumentExtra *UserDynamicData::extraDocument() const
{
    if (d->m_Type != ExtraDocument)
        return 0;
    if (!d->m_Doc)
        d->m_Doc = Print::TextDocumentExtra::fromXml(d->m_Value.toString());
    return d->m_Doc;
}

/**
  \brief Returns the value of the dynamic data into a QVariant.
  Return the tkTextDocumentExtra::toHtml() if this data is a tkTextDocumentExtra.
*/
QVariant UserDynamicData::value() const
{
    if (d->m_Type==ExtraDocument) {
        if (!d->m_Doc)
            d->m_Doc = Print::TextDocumentExtra::fromXml(d->m_Value.toString());
        return d->m_Doc->toHtml();
    } else {
        return d->m_Value;
    }
    return QVariant();
}

/**
  \brief Defines the bound values of the QSqlQuery according to the database
  scheme without the Id field.
  \sa tkUserConstants::DATAfields, UserBase::saveUser()
*/
void UserDynamicData::prepareQuery(QSqlQuery &bindedQuery) const
{
    bindedQuery.bindValue(DATA_USER_UUID,  d->m_UserUuid);
    bindedQuery.bindValue(DATA_DATANAME ,  d->m_Name);

    if (d->m_Name==Constants::USER_DATA_PREFERENCES) {
        bindedQuery.bindValue(DATA_STRING ,    QVariant());
        bindedQuery.bindValue(DATA_LONGSTRING, QVariant());
        bindedQuery.bindValue(DATA_FILE,       d->m_Value);
        bindedQuery.bindValue(DATA_NUMERIC,    QVariant());
        bindedQuery.bindValue(DATA_DATE,       QVariant());
    } else {
        switch (d->m_Value.type())
        {
        case QVariant::DateTime :
        {
        bindedQuery.bindValue(DATA_STRING ,    QVariant());
        bindedQuery.bindValue(DATA_LONGSTRING, QVariant());
        bindedQuery.bindValue(DATA_FILE,       QVariant());
        bindedQuery.bindValue(DATA_NUMERIC,    QVariant());
        bindedQuery.bindValue(DATA_DATE,       d->m_Value);
            break;
        }
        case QVariant::Double :
        case QVariant::Int :
        {
        bindedQuery.bindValue(DATA_STRING ,    QVariant());
        bindedQuery.bindValue(DATA_LONGSTRING, QVariant());
        bindedQuery.bindValue(DATA_FILE,       QVariant());
        bindedQuery.bindValue(DATA_NUMERIC,    d->m_Value);
        bindedQuery.bindValue(DATA_DATE,       QVariant());
            break;
        }
        default:
        {
            QString tmp = d->m_Value.toString();
            if (type() == ExtraDocument)
                tmp = d->m_Doc->toXml();
            if (tmp.length() < 200) {
        bindedQuery.bindValue(DATA_STRING ,    tmp);
        bindedQuery.bindValue(DATA_LONGSTRING, QVariant());
        bindedQuery.bindValue(DATA_FILE,       QVariant());
            } else if (tmp.length() < 2000) {
        bindedQuery.bindValue(DATA_STRING ,    QVariant());
        bindedQuery.bindValue(DATA_LONGSTRING, tmp);
        bindedQuery.bindValue(DATA_FILE,       QVariant());
            } else {
        bindedQuery.bindValue(DATA_STRING ,    QVariant());
        bindedQuery.bindValue(DATA_LONGSTRING, QVariant());
        bindedQuery.bindValue(DATA_FILE,       tmp);
            }
        bindedQuery.bindValue(DATA_NUMERIC,    QVariant());
        bindedQuery.bindValue(DATA_DATE,       QVariant());
            break;
        }
        }
    }
    bindedQuery.bindValue(DATA_LANGUAGE,   d->m_Language);
    bindedQuery.bindValue(DATA_LASTCHANGE, d->m_Lastchange);
    bindedQuery.bindValue(DATA_TRACE_ID,   d->m_Trace);
}

/** \brief For debugging purpose only */
void UserDynamicData::warn() const
{
    qWarning() << "WARNING UDD" << debugText();
}

/** \brief For debugging purpose only */
QString UserDynamicData::debugText() const
{
    QStringList tmp;
    tmp << "UserDynamicData(";
    tmp << QString("Id: %1").arg(id());
    tmp << QString("UserUuid: %1").arg(d->m_UserUuid);
    tmp << QString("Name: %1").arg(name());
    tmp << QString("Type: %1").arg(type());
    tmp << QString("Size: %1").arg(value().toString().size());
    tmp << QString("Lang: %1").arg(d->m_Language);
    tmp << QString("Dirty: %1").arg(isModified()?"yes":"no");
    tmp << QString("Null: %1").arg(isNull()?"yes":"no");
    return QString(tmp.join("\n               ") + ")");
}

/** Check equality between another UserPlugin::Internal::UserDynamicData. */
bool UserDynamicData::operator==(const UserDynamicData &other) const
{
    if (&other==this)
        return true;
    return (other.d->m_Name == d->m_Name &&
            other.d->m_IsDirty == d->m_IsDirty &&
            other.d->m_IsNull == d->m_IsNull &&
            other.d->m_Id == d->m_Id &&
            other.d->m_Trace == d->m_Trace &&
            other.d->m_UserUuid == d->m_UserUuid &&
            other.d->m_Value == d->m_Value &&
            other.d->m_Language == d->m_Language &&
            other.d->m_Lastchange == d->m_Lastchange &&
            other.d->m_Type == d->m_Type &&
            other.d->m_Doc == d->m_Doc
            );
}

namespace UserPlugin {
namespace Internal {

class UserDataPrivate
{
public:
    static QHash<QString, int> m_Link_PaperName_ModelIndex;  /** \brief For speed improvements, stores the link between name of headers/footers/watermark and their index into UserModel \sa UserConstants. */

    UserDataPrivate() :
        m_Editable(false),
        m_Modified(false),
        m_IsNull(false),
        m_IsCurrent(false),
        m_HasModifiedDynamicData(false),
        m_PasswordChanged(false)
    {
        if (m_Link_PaperName_ModelIndex.count() == 0)
            feedStaticHash();
    }

    void feedStaticHash()
    {
        m_Link_PaperName_ModelIndex.insert(USER_DATA_GENERICHEADER, Core::IUser::GenericHeader);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_GENERICFOOTER, Core::IUser::GenericFooter);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_GENERICWATERMARK, Core::IUser::GenericWatermark);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_ADMINISTRATIVEHEADER, Core::IUser::AdministrativeHeader);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_ADMINISTRATIVEFOOTER, Core::IUser::AdministrativeFooter);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_ADMINISTRATIVEWATERMARK, Core::IUser::AdministrativeWatermark);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_PRESCRIPTIONHEADER, Core::IUser::PrescriptionHeader);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_PRESCRIPTIONFOOTER, Core::IUser::PrescriptionFooter);
        m_Link_PaperName_ModelIndex.insert(USER_DATA_PRESCRIPTIONWATERMARK, Core::IUser::PrescriptionWatermark);
    }

    ~UserDataPrivate()
    {
        qDeleteAll(m_DynamicData);
        m_DynamicData.clear();
    }

    /**
      \brief Transform dynamic data name to model index for headers/footers and watermark
      \sa tkUserConstants, setDynamicData()
    */
    int documentNameToIndex(const char*name)
    {
        QString tmp = QString(name);
        Q_ASSERT(tmp.startsWith(PAPERS_MARK));
        Q_ASSERT(m_Link_PaperName_ModelIndex.keys().contains(name));
        return m_Link_PaperName_ModelIndex.value(name);
    }

    /**
      \brief Transform dynamic data model index for headers/footers and watermark to its name
      \sa tkUserConstants, setDynamicData()
    */
    QString documentIndexToName(const int id)
    {
        Q_ASSERT(m_Link_PaperName_ModelIndex.values().contains(id));
        return m_Link_PaperName_ModelIndex.key(id);
    }

//    /** \brief Logs the last changed date of a dynamic data to now. Mark user as non null and modified. */
//    void logLastChangeDate(const char *name)
//    {
//        m_DataName_Data[name].insert(DATAS_LASTCHANGE, QDateTime::currentDateTime());
//        m_IsNull = false;
//        m_Modified = true;
//        // store modified dynamic data changed into a list to limit database access
//        if (!m_ModifiedDynamicData.contains(name))
//            m_ModifiedDynamicData << name;
//    }

//    /** \brief When add or set dynamic data to user, if it is a known extra document (tkTextDocumentExtra), this part manages its creation into the cache. */
//    void createExtraDoc(const char*name, const QVariant &val, bool logDate)
//    {
//        m_Docs.insert(documentNameToIndex(name), tkTextDocumentExtra::fromXml(val.toString()));
//        if (!m_DataName_Data.contains(name)) {
//            m_DataName_Data[name].insert(DATAS_DATANAME, name);
//            m_DataName_Data[name].insert(DATAS_ID, -1);
//        }
//        if (logDate)
//            logLastChangeDate(name);
//    }


public:
    QHash< int, QHash<int, QVariant > >       m_Table_Field_Value;
    QHash< QString, QHash<int, QVariant >  >  m_Role_Rights;
//    QHash< QString, QHash< int, QVariant > >  m_DataName_Data;
    bool  m_Editable, m_Modified,  m_IsNull, m_IsCurrent;
    QSet< QString > m_ModifiedRoles;
    QHash<QString, UserDynamicData*> m_DynamicData;
    bool m_HasModifiedDynamicData;
    QString m_ClearPassword;
    bool m_PasswordChanged;
    Utils::PasswordCrypter crypter;
};

}  // End Internal
}  // End namespace UserPlugin


QHash<QString, int> UserDataPrivate::m_Link_PaperName_ModelIndex;

/**
 * Construct a default UserData object with:
 * \li automatic uuid setting to a fresh new one
 * \li rights for UserManager are set to Core::IUser::ReadOwn and Core::IUser::WriteOwn
 * \li no rights for Medical, paramedical, dosage management
 * \li empty password is set encrypted
 * \li user is editable
 * \li user uuid is defined but no control (duplicate in database can exist)
 * \li locker is unset
 */
UserData::UserData() :
    d(0)
{
    d = new UserDataPrivate();
    d->m_Editable = true;
    setValue(Table_USERS, USER_ID, -1);
    setValue(Table_USERS, USER_ISVIRTUAL, false);
    setRights(USER_ROLE_USERMANAGER, Core::IUser::ReadOwn | Core::IUser::WriteOwn);
    setRights(USER_ROLE_MEDICAL, Core::IUser::NoRights);
    setRights(USER_ROLE_DOSAGES, Core::IUser::NoRights);
    setRights(USER_ROLE_PARAMEDICAL, Core::IUser::NoRights);
    setRights(USER_ROLE_SECRETARY, Core::IUser::NoRights);
    setRights(USER_ROLE_AGENDA, Core::IUser::NoRights);
    setCryptedPassword(d->crypter.cryptPassword(""));
    setLocker(false);
    createUuid();
    d->m_IsNull = true;
    d->m_IsCurrent = false;
//    setDynamicDataValue(USER_DATAS_LOGINHISTORY,
//                        QCoreApplication::translate("tkUser", "User created at %1\n")
//                        .arg(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate)));
    setModified(false);
}

/**
  \brief Constructor with a determined uuid
  Some default values are set :
  \li rights for UserManager are set to Core::IUser::ReadOwn and Core::IUser::WriteOwn
  \li no rights for Medical, paramedical, dosage management
  \li empty password is set encrypted
  \li user is editable
  \li user uuid is defined but no control (duplicate in database can exist)
  \li locker is unset
 */
UserData::UserData(const QString & uuid)
{
    // TODO : add a uuid checker before all
    d = new UserDataPrivate();
    d->m_Editable = true;
    setValue(Table_USERS, USER_ID, -1);
    setValue(Table_USERS, USER_ISVIRTUAL, false);
    setUuid(uuid);
    setRights(USER_ROLE_USERMANAGER, Core::IUser::ReadOwn | Core::IUser::WriteOwn);
    setRights(USER_ROLE_MEDICAL, Core::IUser::NoRights);
    setRights(USER_ROLE_DOSAGES, Core::IUser::NoRights);
    setRights(USER_ROLE_PARAMEDICAL, Core::IUser::NoRights);
    setRights(USER_ROLE_SECRETARY, Core::IUser::NoRights);
    setCryptedPassword(d->crypter.cryptPassword(""));
    setLocker(false);
    d->m_IsNull = true;
    d->m_IsCurrent = false;
//    setDynamicDataValue(USER_DATAS_LOGINHISTORY,
//                        QCoreApplication::translate("tkUser", "User created at %1\n")
//                        .arg(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate)));
    setModified(false);
}

/** \brief Destructor */
UserData::~UserData()
{
    if (d)
        delete d;
    d=0;
}

/**
 * Define the editable state of the object.
 * Toggling editable state does not affect the modified state of the object.
 * \sa isEditable(), iseditable()
 */
void UserData::setEditable(bool state)
{
    d->m_Editable = state;
}

bool UserData::isEditable() const
{
    return d->m_Editable;
}

/** Change the modification state */
void UserData::setModified(bool state)
{
    d->m_Modified = state;
    if (!state) {
        foreach(UserDynamicData *data, this->modifiedDynamicData()) {
            data->setModified(false);
        }
        d->m_ModifiedRoles.clear();
        d->m_PasswordChanged = false;
    }
}

/** Returns the modification state */
bool UserData::isModified() const
{
    if (d->m_Modified)
        return true;
    if (d->m_PasswordChanged)
        return true;
    if (hasModifiedDynamicDataToStore())
        return true;
    if (hasModifiedRightsToStore())
        return true;
    return false;
}

bool UserData::isPasswordModified() const
{
    return d->m_PasswordChanged;
}

void UserData::setPasswordModified(bool state)
{
    d->m_PasswordChanged = state;
}

/** Returns true if the object is null */
bool UserData::isNull() const
{
    return d->m_IsNull;
}

/** Returns true if the object is empty */
bool UserData::isEmpty() const
{
    if (id() != -1)
        return false;
    if (d->m_Table_Field_Value.count() == 1)
        return true;
    return false;
}

/**
 * Set this user as the current.
 * This state is modified by the UserPlugin::Internal::UserModel
 */
void UserData::setCurrent(bool state)
{
    d->m_IsCurrent = state;
}

/**
 * Returns true if this user was defined as the current one.
 * This state is modified by the UserPlugin::Internal::UserModel
 */
bool UserData::isCurrent() const
{
    return d->m_IsCurrent;
}

/**
 * \brief If user is editable and does not have an uuid then create a new one
 *  and return true, otherwise return false.
 */
bool UserData::createUuid()
{
    if (!d->m_Editable)
        return false;
    if (!uuid().isEmpty())
        return true;
    setUuid(Utils::createUid());
    return true;
}

/** \brief Defines the Uuid of the user. Modify all users' data uuid. */
void UserData::setUuid(const QString & val)
{
    Q_ASSERT(!val.isEmpty());
    setValue(Table_USERS, USER_UUID, val);
    foreach(UserDynamicData *dyn, d->m_DynamicData)
        dyn->setUserUuid(val);
}

//------------------------------------------------------------------------------
//---------------------------------- Setters -----------------------------------
//------------------------------------------------------------------------------
/**
 * Reserved for database feeding. Only UserBase should use it. \n
 * If user is editable, set the value \e val for the table \e tableref index
 * and \e fieldref field index. \n
 * Values must be determined fields values not dynamicValues. \n
 * When a value is defined the modified state of the object set to \e true.
 * \sa UserPlugin::Constants::Tables
*/
void UserData::setValue(const int tableref, const int fieldref, const QVariant &val)
{
    if (!d->m_Editable)
        return;

    // Manage password
    if (tableref == Table_USERS && fieldref == USER_PASSWORD) {
        setCryptedPassword(val);
        return;
    }

    // Add to internal values if is not already defined
    if (d->m_Table_Field_Value.count()) {
        if (d->m_Table_Field_Value.keys().contains(tableref)) {
            const QHash<int, QVariant> &table = d->m_Table_Field_Value.value(tableref);
            if (table.keys().contains(fieldref))
                if (table.value(fieldref) == val)
                    return;
        }
    }
    d->m_Table_Field_Value[tableref].insert(fieldref, val);
    d->m_IsNull = false;
    setModified(true);
}

/**
 \brief This member is reserved for database feeding. Only UserBase should use it. \n
 If user is editable, set the dynamic value \e val named \e name and \e fieldref field index.
 \todo explain dynamicValues / values
*/
void UserData::addDynamicDataFromDatabase(const QList<UserDynamicData*> &list)
{
    if (!d->m_Editable)
        return;
    d->m_IsNull = false;
    foreach(UserDynamicData *dyn, list) {
        // no double
        if (!d->m_DynamicData.keys().contains(dyn->name()))
            d->m_DynamicData.insert(dyn->name(), dyn);
    }
}

/**
 \brief This member is reserved for database feeding. Only UserBase should use it. \n
 If user is editable, set the dynamic value \e val named \e name and \e fieldref field index.
 \todo explain dynamicValues / values
*/
void UserData::addRightsFromDatabase(const char *roleName, const int fieldref, const QVariant & val)
{
    if (!d->m_Editable)
        return;
    if (fieldref == RIGHTS_USER_UUID)// don't store user's uuid
        return;

    // data are stored into a QHash<int, QHash< int, QVariant > >  int is the row
    d->m_Role_Rights[roleName].insert(fieldref, val);
    d->m_IsNull = false;
    setModified(true);
}

/**
 * \brief Stores into this object cache the dynamic data of users.
 * This member is purely private.
 * Mark the UserDynamicData as dirty if value \e val differs of the stored data.
*/
void UserData::setDynamicDataValue(const char *name, const QVariant &val, UserDynamicData::DynamicDataType t)
{
    Q_UNUSED(t);
    // this member should not be used by database
    if (!val.isValid())
        return;
    if (!d->m_Editable)
        return;

    // empy or null qvariant into a non existing dynamic data --> return
    if ((val.isNull()) ||
         (((val.type() == QVariant::String) || (val.type() == QVariant::StringList))
           && (val.toString().isEmpty()))) {
        if (!d->m_DynamicData.keys().contains(name))
            return;
    }

    if (!d->m_DynamicData.keys().contains(name)) {
        UserDynamicData *data = new UserDynamicData();
        data->setName(name);
        data->setUserUuid(uuid());
        d->m_DynamicData.insert(name,data);
    }
    UserDynamicData *data = d->m_DynamicData[name];
    data->setValue(val);

//    qWarning() << d->m_DynamicData;
}

/**
  Define the rights of the user according to the role name of the rights.
  \sa UserPlugin::Constants::USER_ROLE_SECRETARY,
  UserPlugin::Constants::USER_ROLE_MEDICAL,
  UserPlugin::Constants::USER_ROLE_PARAMEDICAL,
  UserPlugin::Constants::USER_ROLE_USERMANAGER,
  UserPlugin::Constants::USER_ROLE_DOSAGES,
  Core::IUser::UserRights
*/
void UserData::setRights(const char *roleName, const Core::IUser::UserRights rights)
{
    Core::IUser::UserRights r = rights;
    if (rights & Core::IUser::ReadAll)
        r |= Core::IUser::ReadOwn | Core::IUser::ReadDelegates;
    if (rights & Core::IUser::WriteAll)
        r |= Core::IUser::WriteOwn | Core::IUser::WriteDelegates;
    d->m_Role_Rights[roleName].insert(RIGHTS_RIGHTS, int(r));
    if (!d->m_ModifiedRoles.contains(roleName))
        d->m_ModifiedRoles.insert(roleName);
    d->m_IsNull = false;
    setModified(true);
}

/**
  Define the password is a clear way, the crypted password is sync if needed
  \sa setCryptedPassword(),
*/
void UserData::setClearPassword(const QString &val)
{
    if (val == d->m_ClearPassword)
        return;
    d->m_ClearPassword = val;
    d->m_PasswordChanged = true;
    // Sync cryptedPassword
    if (d->crypter.cryptPassword(val) != cryptedPassword()) {
        setCryptedPassword(d->crypter.cryptPassword(val));
    }
}

/** Return the translated human readable title of the user. */
QString UserData::title() const
{
    return Trans::ConstantTranslations::titles().at(titleIndex());
}

/** Return the translated human readable gender of the user. */
QString UserData::gender() const
{
    return Trans::ConstantTranslations::genders().at(genderIndex());
}

/** Defines the photo of the user */
void UserData::setPhoto(const QPixmap &pix)
{
    setDynamicDataValue(USER_DATA_PHOTO, Utils::pixmapToBase64(pix));
}

/** Defines the photo of the user */
QPixmap UserData::photo() const
{
    return Utils::pixmapFromBase64(dynamicDataValue(USER_DATA_PHOTO).toByteArray());
}

/**
 * Define the crypted password. You should not use this function,
 * but the setClearPassword() instead.
 * \sa setClearPassword(), isPasswordModified(), cryptedPassword(), clearPassword()
*/
void UserData::setCryptedPassword(const QVariant &val)
{
    if (val.toString() == value(Table_USERS, USER_PASSWORD).toString())
        return;
    d->m_Table_Field_Value[Table_USERS].insert(USER_PASSWORD, val);
    d->m_PasswordChanged = true;
}

/**
 * This function needs to be rewritten, see issue #61
 * Add the current login to the login history of the user.
 */

/**
void UserData::addLoginToHistory()
{
    setDynamicDataValue(USER_DATA_LOGINHISTORY,
                        QString("%1 %2")
                        .arg(dynamicDataValue(USER_DATA_LOGINHISTORY).toString())
                        .arg(QCoreApplication::translate("tkUser", "User logged at %1 \n")
                             .arg(lastLoggedIn().toString(Qt::DefaultLocaleLongDate)))
              );
    setModified(true);
}
*/

QString UserData::decryptedLogin() const
{
    return Utils::loginFromSQL(value(Table_USERS, USER_LOGIN));
}

//------------------------------------------------------------------------------
//---------------------------------- Getters -----------------------------------
//------------------------------------------------------------------------------
/**
 * Return the value corresponding to the
 * \e tableref and \e fieldref of the database scheme.
 * \sa setValue()
*/
QVariant UserData::value(const int tableref, const int fieldref) const
{
    if (d->m_Table_Field_Value.keys().contains(tableref)) {
        const QHash<int, QVariant> &fields = d->m_Table_Field_Value.value(tableref);
        if (fields.contains(fieldref))
            return fields.value(fieldref);
    }
    return QVariant();
}

/**
  \brief Returns the content of the dynamic data value named \e name.
*/
QVariant UserData::dynamicDataValue(const char*name) const
{
    if (!d->m_DynamicData.keys().contains(name))
        return QVariant();
    return d->m_DynamicData.value(name)->value();
}

/**
 * Returns the database right value associated with
 * the \e name and \e fieldref of the internal table
 * \sa UserPlugin::Internal::UserBase, UserPlugin::Constants::RIGHTSfields
*/
QVariant UserData::rightsValue(const QString &name, const int fieldref) const
{
//    qWarning() << "rightsValue" << name << fieldref << d->m_Role_Rights.value(name).value(fieldref);
    return d->m_Role_Rights.value(name).value(fieldref);
}

/**
 * Returns the right value associated with the uid \e name
 * \sa Core::IUser::UserRight
*/
QVariant UserData::rightsValue(const char *name) const
{
    return d->m_Role_Rights.value(name).value(RIGHTS_RIGHTS);
}

/**
 * Returns \e true if the user has the right \e rightToTest
 * associated with the uid \e name
 * \sa Core::IUser::UserRight
*/
bool UserData::hasRight(const char *name, const int rightToTest) const
{
    Core::IUser::UserRights rights = Core::IUser::UserRights(rightToTest);
    Core::IUser::UserRights rightrole = Core::IUser::UserRights(rightsValue(name).toInt());
    return (rightrole & rights);
}

/** \brief Return true if there are modifications to save in the dynamicData. */
bool UserData::hasModifiedDynamicDataToStore() const
{
    return modifiedDynamicData().count();
}

/** \brief Return the list of modified dynamicData. The returned pointer \b should \b not \b be \b deleted. */
QList<UserDynamicData*> UserData::modifiedDynamicData() const
{
    QList<UserDynamicData*> list;
    foreach(UserDynamicData *dyn, d->m_DynamicData.values()) {
        if (dyn->isModified()) {
            list << dyn;
        }
    }
    return list;
}

/** \brief Return true if there are modifications to save in the rights. */
bool UserData::hasModifiedRightsToStore() const
{
    return d->m_ModifiedRoles.count();
}

/** \brief Return the list of modified rights roles. */
QStringList UserData::modifiedRoles() const
{
    return d->m_ModifiedRoles.toList();
}

/**
 * Return the uncrypted password. Once the user is saved into the database,
 * when you get it from the database, this uncrypted password is not available
 * anymore.
 * \sa UserPlugin::Internal::UserBase::saveUser()
 */
QString UserData::clearPassword() const
{
    return d->m_ClearPassword;
}

// HEADER/FOOTER/WATERMARK MANAGEMENT

/**
 * Set the paper for this user.
 * The \e index param refers to
 * Core::IUser::{Generic,Administrative,Prescription}{Header,Footer,Watermark}
 * \sa Print::TextDocumentExtra, Print::TextDocumentExtra
 */
void UserData::setExtraDocument(Print::TextDocumentExtra *extra, const int index)
{
    if (!extra)
        return;
    QString name = d->documentIndexToName(index);
    Q_ASSERT(!name.isEmpty());
    if (name.isEmpty())
        return;
    if (!d->m_DynamicData.keys().contains(name)) {
        UserDynamicData *data = new UserDynamicData();
        data->setName(name);
        data->setUserUuid(uuid());
        d->m_DynamicData.insert(name, data);
    }
    d->m_DynamicData[name]->setValue(extra);
    d->m_DynamicData[name]->setModified(true);
}

/**
 * Defines the header, footer and watermark to use for the print processes.
 * The \e index param refers to
 * Core::IUser::{Generic,Administrative,Prescription}{Header,Footer,Watermark}
 * \sa Print::TextDocumentExtra, Print::TextDocumentExtra::toHtml()
*/
void UserData::setExtraDocumentHtml(const QVariant &val, const int index)
{
    QString name = d->documentIndexToName(index);
    Q_ASSERT(!name.isEmpty());
    if (name.isEmpty())
        return ;
    if (!d->m_DynamicData.keys().contains(name)) {
        UserDynamicData *data = new UserDynamicData();
        data->setName(name);
        data->setUserUuid(uuid());
        d->m_DynamicData.insert(name,data);
    }
    d->m_DynamicData[name]->setValue(val);
    d->m_DynamicData[name]->setModified(true);
}

void UserData::setExtraDocumentPresence(const int presence, const int index)
{
    QString name = d->documentIndexToName(index);
    Q_ASSERT(!name.isEmpty());
    if (name.isEmpty())
        return ;
    if (!d->m_DynamicData.keys().contains(name)) {
        UserDynamicData *data = new UserDynamicData();
        data->setName(name); // define type as well
        data->setUserUuid(uuid());
        d->m_DynamicData.insert(name,data);
    }
    Print::TextDocumentExtra *t = d->m_DynamicData.value(name)->extraDocument();
    t->setPresence(Print::Printer::Presence(presence));
    d->m_DynamicData[name]->setModified(true);
}

QVariant UserData::extraDocumentHtml(const int index) const
{
    QString name = d->documentIndexToName(index);
    Q_ASSERT(!name.isEmpty());
    if (name.isEmpty())
        return QVariant();

    if (d->m_DynamicData.keys().contains(name)) {
        if (d->m_DynamicData.value(name)->type() == UserDynamicData::ExtraDocument)
            return d->m_DynamicData.value(name)->value();
    }
    return QVariant();
}

/**
 * Returns the paper to use during the print process for this user.
 * The \e index param refers to
 * Core::IUser::{Generic,Administrative,Prescription}{Header,Footer,Watermark}
 * \sa Print::TextDocumentExtra, Print::TextDocumentExtra
 */
Print::TextDocumentExtra *UserData::extraDocument(const int index) const
{
    QString name = d->documentIndexToName(index);
    Q_ASSERT(!name.isEmpty());
    if (name.isEmpty())
        return 0;

    if (d->m_DynamicData.keys().contains(name)) {
        if (d->m_DynamicData.value(name)->type() == UserDynamicData::ExtraDocument)
            return d->m_DynamicData.value(name)->extraDocument();
    }
    return 0;
}

/** Returns a computed full name including title, usual name, firstname and other names */
QString UserData::fullName() const
{
    QString r = title() + " " + usualName() + " " + otherNames() + " " + firstname();
    r.replace("  ", " ");
    return r;
}

//---------------------------------------------------------------------------------------
//----------------------------------- Debug ---------------------------------------------
//---------------------------------------------------------------------------------------
QString UserData::debugText() const
{
    QStringList s;
    s << uuid();
    if (isEmpty())
        s << "empty";
    if (isNull())
        s << "null";
    if (isCurrent())
        s << "current";
    if (isEditable())
        s << "editable";
    if (isModified())
        s << "modified";

    // Fields value
    for (int i = 0; i < USER_MaxParam; i++)
        s << tkTr(Trans::Constants::_1_COLON_2)
        .arg(userBase()->fieldName(Table_USERS , i))
        .arg(d->m_Table_Field_Value.value(Table_USERS).value(i).toString());

    // Modified dynamic data uids
    if (!hasModifiedDynamicDataToStore()) {
        s << "no modified dynamic data";
    } else {
        const QList<UserDynamicData*> &dynList = modifiedDynamicData();
        foreach(const UserDynamicData *dyn, dynList) {
            s << QString("modified dynamic data: %1").arg(dyn->name());
        }
    }
    // Modified rights
    if (!hasModifiedRightsToStore()) {
        s << "no modified rights";
    } else {
        s << "modified rights";
    }

    return QString("UserData(%1\n           )").arg(s.join(",\n           "));

    /*
    const QList<UserDynamicData*> &dynList = modifiedDynamicData();
    foreach(const UserDynamicData *dyn, d->m_DynamicData.values()) {
        tmp += "\nDATA: " + dyn->name() + "\n";
        tmp += QString("%1\n").arg(dyn->warnText());
    }
    list << tmp;
    tmp.clear();

    if (d->m_Role_Rights.count() == 0)
        list <<  "    /!\\ NO RIGHTS RECORDED /!\\ " ;
    else
        foreach(const QString & id, d->m_Role_Rights.keys()){
            tmp += "\n\nRIGHT: " + id + "\n";
            for (i = 0; i < RIGHTS_MaxParam; i++)
                tmp += QString("%1: %2 = %3\n")
                                   .arg(id)
                                   .arg(userBase()->fieldName(Table_RIGHTS , i))
                                   .arg(d->m_Role_Rights.value(id).value(i).toString());
        }
    list << tmp;
    tmp.clear();

    tmp += "  *Values *\n";
    tmp += QString("%1 = %2\n").arg("Editable").arg(isEditable());
    tmp += QString("%1 = %2\n").arg("m_Modified").arg(isModified());
    tmp += QString("%1 = %2\n").arg("m_IsNull").arg(d->m_IsNull);
    tmp += QString("%1 = %2\n").arg("hasModifiedDynamicData").arg(hasModifiedDynamicDataToStore());
    tmp += "modifiedDynamicData = ";
    foreach(UserDynamicData *dyn, dynList)
             tmp += dyn->name() + "; ";
    tmp.chop(2);
    tmp += "\n";
    list << tmp;
    return list;
    */
}

QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserData &a)
{
    dbg.nospace() << a.debugText();
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserData *c)
{
    if (!c) {
        dbg.nospace() << "UserData(0x0)";
        return dbg.space();
    }
    dbg.nospace() << c->debugText();
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserDynamicData &a)
{
    dbg.nospace() << a.debugText();
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const UserPlugin::Internal::UserDynamicData *c)
{
    if (!c) {
        dbg.nospace() << "UserDynamicData(0x0)";
        return dbg.space();
    }
    dbg.nospace() << c->debugText();
    return dbg.space();
}
