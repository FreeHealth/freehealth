/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#include "tkUser.h"

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkSerializer.h>
#include <tkTextDocumentExtra.h>

// include usertoolkit headers
#include <tkUserBase.h>
#include <tkUserGlobal.h>

// include Qt headers
#include <QApplication>
#include <QDateTime>
#include <QUuid>
#include <QHash>
#include <QVariant>
#include <QSet>

Q_TKUSER_USING_CONSTANTS

/**
  \brief Private part
  \internal
*/
class UserDynamicDataPrivate {
public:
    UserDynamicDataPrivate() : m_IsDirty(false), m_IsNull(true), m_Id(-1), m_Trace(-1), m_Doc(0), m_Type(UserDynamicData::String)
    {
        m_Language = QLocale().name().left(2);
    }

    ~UserDynamicDataPrivate() {}

    /** \brief Defines the Html content of the extraDocument */
    void setDocumentHtml(const QVariant &value)
    {
        if (!m_Doc)
            m_Doc = new tkTextDocumentExtra();
        m_Doc->setHtml(value.toString() );
    }

    /** \brief Defines the Xml of the extraDocument */
    void setDocumentXml(const QVariant &value)
    {
        if (!m_Doc)
            m_Doc = new tkTextDocumentExtra();
        m_Doc = tkTextDocumentExtra::fromXml(value.toString() );
    }

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
    QVariant m_Value;     /*!< \brief Value stored into a QVariant. Setted to null for tkTextDocumentExtra. */
    QString m_Language;   /*!< \brief Language compatible */
    QDateTime m_Lastchange;                     /*!< \brief Date of the last change. */
    UserDynamicData::DynamicDataType m_Type;    /*!< \brief Type of the UserDynamicData. \sa */
    tkTextDocumentExtra *m_Doc;                 /*!< \brief Defined if the UserDynamicData is a tkTextDocumentExtra. Pointer is deleted in the destructor. */
};

UserDynamicData::UserDynamicData() : d(0)
{
    d = new UserDynamicDataPrivate();
}

UserDynamicData::~UserDynamicData()
{
    if (d) delete d; d=0;
}

/** \brief Returns true if the data is actually null */
bool UserDynamicData::isNull() const
{
    return d->m_IsNull;
}

/** \brief Returns to if the value has change since last call of feedFromSql(). */
bool UserDynamicData::isDirty() const
{
    return d->m_IsDirty;
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
}

/** \brief Returns the Id of the UserDynamicData */
int UserDynamicData::id() const
{
    return d->m_Id;
}

/** \brief Defines the related user's uuid of UserDynamicData */
void UserDynamicData::setUserUuid(const QString &uuid)
{
    d->m_UserUuid = uuid;
}

/** \brief Feed the class with datas without marking it as dirty. Only used by tkUserBase. */
void UserDynamicData::feedFromSql(const int field, const QVariant& value )
{
    Q_ASSERT(field>=DATAS_ID && field <= DATAS_TRACE_ID);
    switch (field) {
            case DATAS_ID : d->m_Id = value.toInt();
            case DATAS_USER_UUID : d->m_UserUuid = value.toString(); break;
            case DATAS_DATANAME:
                {
                    d->m_Name = value.toString();
                    if (d->m_Name.startsWith(PAPERS_MARK)) {
                        d->m_Type = ExtraDocument;
                    }
                    break;
                }
            case DATAS_LANGUAGE: d->m_Language = value.toString(); break;
            case DATAS_LASTCHANGE: d->m_Lastchange = value.toDateTime(); break;
            case DATAS_TRACE_ID: d->m_Trace = value.toInt(); break;
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
                            qWarning() << d->m_Name << "datetime";
                        else if (value.type() == QVariant::String)
                            qWarning() << d->m_Name << "string";
                        d->m_Value = value;
                    }
                    break;
                }
            }
    d->m_IsNull = false;
}

/** \brief Feed the class with datas collected into a QHash. Only used by tkUserBase. */
void UserDynamicData::feedFromSql(const QHash<int,QVariant> &values)
{
    foreach(const int k, values.keys())
        feedFromSql(k, values.value(k) );
    d->m_IsNull = false;
}

/** \brief Defines this dynamic data as a tkTextDocumentExtra. */
void UserDynamicData::setValue(tkTextDocumentExtra *extra )
{
    if (d->m_Doc)
        delete d->m_Doc;
    d->m_Doc = extra;
    d->m_Value = QVariant();
    d->setDirty();
}

/** \brief Defines the values of the dynamic data. If it is a tkTextDocumentExtra defines the html code. */
void UserDynamicData::setValue(const QVariant &value )
{
    if (d->m_Type==ExtraDocument) {
        d->setDocumentHtml(value);
    } else if (d->m_Value != value) {
        d->m_Value = value;
        d->setDirty();
    }
}

/**
  \brief Return the tkTextdocumentExtra related to this user dynamic data or 0 if this data does not correspond to a tkTextDocuementExtra.
  The returned pointer SHOULD NOT BE deleted !!!
  \sa UserDynamicData::DynamicDataType
*/
tkTextDocumentExtra *UserDynamicData::extraDocument() const
{
    if (!d->m_Type==ExtraDocument)
        return 0;
    if (!d->m_Doc)
        d->m_Doc = tkTextDocumentExtra::fromXml(d->m_Value.toString());
    return d->m_Doc;
}

/**
  \brief Returns the value of the dynamic data into a QVariant.
  Return the tkTextDocumentExtra::toHtml() if this data is a tkTextDocumentExtra.
*/
QVariant UserDynamicData::value() const
{
//    qWarning() << "UserDynamicData::value() ";
    if (d->m_Type==ExtraDocument) {
        if (!d->m_Doc)
            d->m_Doc = tkTextDocumentExtra::fromXml(d->m_Value.toString());
        return d->m_Doc->toHtml();
    } else {
//        qWarning() << d->m_Name << d->m_Value;
        return d->m_Value;
    }
    return QVariant();
}

/**
  \brief Defines the binded values of the QSqlQuery according to the database scheme without the Id field.
  \sa tkUserConstants::DATASfields, tkUserBase::saveUser()
*/
void UserDynamicData::prepareQuery(QSqlQuery &bindedQuery) const
{
    bindedQuery.bindValue( DATAS_USER_UUID,  d->m_UserUuid );
    bindedQuery.bindValue( DATAS_DATANAME ,  d->m_Name );

    switch (d->m_Value.type())
    {
        case QVariant::DateTime :
        {
            bindedQuery.bindValue( DATAS_STRING ,    QVariant());
            bindedQuery.bindValue( DATAS_LONGSTRING, QVariant());
            bindedQuery.bindValue( DATAS_FILE,       QVariant());
            bindedQuery.bindValue( DATAS_NUMERIC,    QVariant());
            bindedQuery.bindValue( DATAS_DATE,       d->m_Value);
            break;
        }
        case QVariant::Double :
        case QVariant::Int :
        {
            bindedQuery.bindValue( DATAS_STRING ,    QVariant());
            bindedQuery.bindValue( DATAS_LONGSTRING, QVariant());
            bindedQuery.bindValue( DATAS_FILE,       QVariant());
            bindedQuery.bindValue( DATAS_NUMERIC,    d->m_Value);
            bindedQuery.bindValue( DATAS_DATE,       QVariant());
            break;
        }
        default:
        {
            if (d->m_Value.toString().length() < 200) {
                bindedQuery.bindValue( DATAS_STRING ,    d->m_Value);
                bindedQuery.bindValue( DATAS_LONGSTRING, QVariant());
                bindedQuery.bindValue( DATAS_FILE,       QVariant());
            } else if (d->m_Value.toString().length() < 2000) {
                bindedQuery.bindValue( DATAS_STRING ,    QVariant());
                bindedQuery.bindValue( DATAS_LONGSTRING, d->m_Value);
                bindedQuery.bindValue( DATAS_FILE,       QVariant());
            } else {
                bindedQuery.bindValue( DATAS_STRING ,    QVariant());
                bindedQuery.bindValue( DATAS_LONGSTRING, QVariant());
                bindedQuery.bindValue( DATAS_FILE,       d->m_Value);
            }
            bindedQuery.bindValue( DATAS_NUMERIC,    QVariant());
            bindedQuery.bindValue( DATAS_DATE,       QVariant());
            break;
        }
    }
    bindedQuery.bindValue( DATAS_LANGUAGE,   d->m_Language );
    bindedQuery.bindValue( DATAS_LASTCHANGE, d->m_Lastchange );
    bindedQuery.bindValue( DATAS_TRACE_ID,   d->m_Trace );
}

/** \brief For debugging purpose only */
void UserDynamicData::warn() const
{
    qWarning() << "WARNING UDD" << warnText();
}

/** \brief For debugging purpose only */
QString UserDynamicData::warnText() const
{
    QString tmp;
    tmp = QString("Name : %1, Type : %2, Value : %3, Lang : %4, Dirt %5 Null %6")
          .arg(d->m_Name)
          .arg(d->m_Type)
          .arg(d->m_Value.toString())
          .arg(d->m_Language)
          .arg(d->m_IsDirty)
          .arg(d->m_IsNull);
    return tmp;
}




/**
  \brief Private part
  \internal
*/
class tkUserPrivate
{
public:
    static QHash<QString, int> m_Link_PaperName_ModelIndex;  /** \brief For speed improvments, stores the link between name of headers/footers/watermark and there index into tkUserModel \sa tkUserConstants. */

    tkUserPrivate()
    {
        if (m_Link_PaperName_ModelIndex.count() == 0)
            feedStaticHash();
    }

    void feedStaticHash()
    {
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_GENERICHEADER, User::UserGenericHeader);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_GENERICFOOTER, User::UserGenericFooter);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_GENERICWATERMARK, User::UserGenericWatermark);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_ADMINISTRATIVEHEADER, User::UserAdministrativeHeader);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_ADMINISTRATIVEFOOTER, User::UserAdministrativeFooter);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_ADMINISTRATIVEWATERMARK, User::UserAdministrativeWatermark);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_PRESCRIPTIONHEADER, User::UserPrescriptionHeader);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_PRESCRIPTIONFOOTER, User::UserPrescriptionFooter);
        m_Link_PaperName_ModelIndex.insert(USER_DATAS_PRESCRIPTIONWATERMARK, User::UserPrescriptionWatermark);
    }

    ~tkUserPrivate()
    {
        qDeleteAll(m_DynamicDatas);
        m_DynamicDatas.clear();
    }

    /**
      \brief Transform dynamic data name to model index for headers/footers and watermark
      \sa tkUserConstants, setDynamicData()
    */
    int documentNameToIndex(const char* name)
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
//        m_DataName_Datas[name].insert(DATAS_LASTCHANGE, QDateTime::currentDateTime() );
//        m_IsNull = false;
//        m_Modified = true;
//        // store modified dynamic datas changed into a list to limit database access
//        if (!m_ModifiedDynamicDatas.contains(name))
//            m_ModifiedDynamicDatas << name;
//    }

//    /** \brief When add or set dynamic data to user, if it is a known extra document (tkTextDocumentExtra), this part manages its creation into the cache. */
//    void createExtraDoc(const char* name, const QVariant &val, bool logDate)
//    {
//        m_Docs.insert(documentNameToIndex(name), tkTextDocumentExtra::fromXml(val.toString() ));
//        if (!m_DataName_Datas.contains(name)) {
//            m_DataName_Datas[name].insert(DATAS_DATANAME, name );
//            m_DataName_Datas[name].insert(DATAS_ID, -1 );
//        }
//        if (logDate)
//            logLastChangeDate(name);
//    }


public:
    QHash< int, QHash<int, QVariant > >       m_Table_Field_Value;
    QHash< QString, QHash<int, QVariant >  >  m_Role_Rights;
//    QHash< QString, QHash< int, QVariant > >  m_DataName_Datas;
    bool  m_Modifiable, m_Modified,  m_IsNull, m_IsCurrent;
    QSet< QString > m_ModifiedRoles;
    QHash<QString, UserDynamicData*> m_DynamicDatas;
    bool m_HasModifiedDynamicDatas;
};

QHash<QString, int> tkUserPrivate::m_Link_PaperName_ModelIndex;

tkUser::tkUser()
    : d(0 )
{
    d = new tkUserPrivate();
    d->m_Modifiable = true;
    setValue(Table_USERS, USER_ID, -1 );
    setRights(USER_ROLE_USERMANAGER, User::ReadOwn | User::WriteOwn );
    setRights(USER_ROLE_MEDICAL, User::NoRights );
    setRights(USER_ROLE_DOSAGES, User::NoRights );
    setRights(USER_ROLE_PARAMEDICAL, User::NoRights );
    setRights(USER_ROLE_ADMINISTRATIVE, User::NoRights );
    setCryptedPassword(tkUserGlobal::crypt(""));
    setLocker(false );
    createUuid();
    d->m_IsNull = true;
    d->m_Modified = false;
    d->m_IsCurrent = false;
    setDynamicDataValue(USER_DATAS_LOGINHISTORY,
                         QCoreApplication::translate("tkUser", "User created at %1\n" )
                         .arg(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate)));
}

tkUser::tkUser(const QString & uuid )
{
    // TODO : add a uuid checker before all
    d = new tkUserPrivate();
    d->m_Modifiable = true;
    setValue(Table_USERS, USER_ID, -1 );
    setUuid(uuid );
    setRights(USER_ROLE_USERMANAGER, User::ReadOwn | User::WriteOwn );
    setRights(USER_ROLE_MEDICAL, User::NoRights );
    setRights(USER_ROLE_DOSAGES, User::NoRights );
    setRights(USER_ROLE_PARAMEDICAL, User::NoRights );
    setRights(USER_ROLE_ADMINISTRATIVE, User::NoRights );
    setCryptedPassword(tkUserGlobal::crypt(""));
    setLocker(false );
    d->m_IsNull = true;
    d->m_Modified = false;
    d->m_IsCurrent = false;
    setDynamicDataValue(USER_DATAS_LOGINHISTORY,
                         QCoreApplication::translate("tkUser", "User created at %1\n" )
                         .arg(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate)));
}

tkUser::~tkUser()
{
    if (d) delete d; d=0;
}

void tkUser::setModifiable(bool state )
{
    d->m_Modifiable = state;
}

bool tkUser::isModifiable() const
{
    return d->m_Modifiable;
}

void tkUser::setModified(bool state )
{
    d->m_Modified = state;
}

bool tkUser::isModified() const
{
    return d->m_Modified;
}

bool tkUser::isNull() const
{
    return d->m_IsNull;
}

bool tkUser::isEmpty() const
{
    if (id() != -1 )
        return false;
    if (d->m_Table_Field_Value.count() == 1 )
        return true;
    return false;
}

void tkUser::setCurrent(bool state )
{
    d->m_IsCurrent = state;
}

bool tkUser::isCurrent() const
{
    return d->m_IsCurrent;
}

/** \brief If user is modifiable and does not have an uuid then create a new one and return true, otherwise return false. */
bool tkUser::createUuid()
{
    if (! d->m_Modifiable )
        return false;
    if (! uuid().isEmpty() )
        return true;
    setUuid(QUuid::createUuid().toString() );
    return true;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Setters -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
 \brief This member is reserved for database feeding. Only tkUserBase should use it. \n
 If user is modifiable, set the value \e val for the table \e tableref index and \e fieldref field index. \n
 Values must be determined fields values not dynamicValues.
*/
void tkUser::setValue(const int tableref, const int fieldref, const QVariant & val )
{
    if (! d->m_Modifiable )
        return;
    // if value is the same as the stored value --> return
    if (d->m_Table_Field_Value.count() )//contains(tableref))
    {
        if (d->m_Table_Field_Value.keys().contains(tableref)) {
            const QHash<int, QVariant> & table = d->m_Table_Field_Value.value(tableref );
            if (table.keys().contains(fieldref))
                if (table.value(fieldref)==  val )
                    return;
        }
    }
    d->m_Table_Field_Value[tableref].insert(fieldref, val );
    d->m_IsNull = false;
    setModified(true );
}

/**
 \brief This member is reserved for database feeding. Only tkUserBase should use it. \n
 If user is modifiable, set the dynamic value \e val named \e name and \e fieldref field index.
 \todo explain dynamicValues / values
*/
void tkUser::addDynamicDatasFromDatabase(const QList<UserDynamicData*> &list)
{
    if (!d->m_Modifiable)
        return;
    d->m_IsNull = false;
    Q_ASSERT_X(d->m_DynamicDatas.count()<=1, "tkUser::addDynamicDatasFromDatabase", "Only call this function once !");
    if (d->m_DynamicDatas.count()) {
        qDeleteAll(d->m_DynamicDatas);
        d->m_DynamicDatas.clear();
    }
    foreach(UserDynamicData *dyn, list)
        d->m_DynamicDatas.insert(dyn->name(), dyn);
}

/**
 \brief This member is reserved for database feeding. Only tkUserBase should use it. \n
 If user is modifiable, set the dynamic value \e val named \e name and \e fieldref field index.
 \todo explain dynamicValues / values
*/
void tkUser::addRightsFromDatabase(const char * roleName, const int fieldref, const QVariant & val )
{
    if (! d->m_Modifiable )
        return;
    if (fieldref == RIGHTS_USER_UUID)// don't store user's uuid
        return;

    // datas are stored into a QHash<int, QHash< int, QVariant > >  int is the row
    d->m_Role_Rights[roleName].insert(fieldref, val );
    d->m_IsNull = false;
    setModified(true );
}

/**
  \brief Stores into cached datas the dynamic datas of users.
  This member SHOULD NEVER BE USED by tkUserBase.
  Mark the UserDynamicData as dirty if value \e val differs of the stored data.
  \todo documentation
*/
void tkUser::setDynamicDataValue(const char * name, const QVariant & val, UserDynamicData::DynamicDataType t )
{
    // this member should not be used by database
    if (!val.isValid())
        return;
    if (!d->m_Modifiable)
        return;

    // empy or null qvariant into a non existing dynamic data --> return
    if ((val.isNull()) ||
         (((val.type() == QVariant::String) || (val.type() == QVariant::StringList))
           && (val.toString().isEmpty()))) {
        if (!d->m_DynamicDatas.keys().contains(name))
            return;
    }

    if (!d->m_DynamicDatas.keys().contains(name)) {
        UserDynamicData *data = new UserDynamicData();
        data->setName(name);
        data->setUserUuid(uuid());
        d->m_DynamicDatas.insert(name,data);
    }
    UserDynamicData *data = d->m_DynamicDatas[name];
    data->setValue(val);

//    qWarning() << d->m_DynamicDatas;
}

/**
  \todo document
*/
void tkUser::setRights(const char * roleName, const User::UserRights rights )
{
    User::UserRights r = rights;
    if (rights & User::ReadAll )
        r |= User::ReadOwn | User::ReadDelegates;
    if (rights & User::WriteAll )
        r |= User::WriteOwn | User::WriteDelegates;
    d->m_Role_Rights[roleName].insert(RIGHTS_RIGHTS, int(r) );
    if (! d->m_ModifiedRoles.contains(roleName) )
        d->m_ModifiedRoles.insert(roleName );
    d->m_IsNull = false;
    setModified(true );
}

/**
  \todo document
*/
void tkUser::addLoginToHistory()
{
    setDynamicDataValue(USER_DATAS_LOGINHISTORY,
                         dynamicDataValue(USER_DATAS_LOGINHISTORY ).toString() +
                         QCoreApplication::translate("tkUser", "User logged at %1\n" )
                         .arg(lastLogin().toString(Qt::DefaultLocaleLongDate) )
                       );
    setModified(true );
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Getters -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \todo document
*/
QVariant tkUser::value(const int tableref, const int fieldref) const
{
    if (d->m_Table_Field_Value.value(tableref ).keys().contains(fieldref))
        return d->m_Table_Field_Value.value(tableref ).value(fieldref );
    return QVariant();
}

/**
  \brief Returns the content of the dynamic data value named \e name.
*/
QVariant tkUser::dynamicDataValue(const char* name) const
{
    if (!d->m_DynamicDatas.keys().contains(name))
        return QVariant();
    return d->m_DynamicDatas.value(name)->value();
}

/**
  \todo document
*/
QVariant tkUser::rightsValue(const QString & name, const int fieldref) const
{
    return d->m_Role_Rights.value(name ).value(fieldref );
}
/**
  \todo document
*/
QVariant tkUser::rightsValue(const char * name) const
{
    return d->m_Role_Rights.value(name ).value(RIGHTS_RIGHTS );
}


//QString tkUser::lastLogin() const
//{
//    if (! value(Table_USERS, USER_LASTLOG ).isValid() )
//        return QCoreApplication::translate("tkUSer", "Never logged" );
//    QDateTime d = value(Table_USERS, USER_LASTLOG ).toDateTime();
//    return d.toString(Qt::DefaultLocaleLongDate );
//}


//bool tkUser::can(UserManagerRights) const
//{
//    // TODO
//    return true;
//}

/** \brief Return true if there are modifications to save in the dynamicDatas. */
bool tkUser::hasModifiedDynamicDatasToStore() const
{
    return modifiedDynamicDatas().count();
}

/** \brief Return the list of modified dynamicDatas'. The returned pointer SHOULD NOT BE deleted. */
QList<UserDynamicData*> tkUser::modifiedDynamicDatas() const
{
    QList<UserDynamicData*> list;
    foreach( UserDynamicData *dyn, d->m_DynamicDatas.values() ) {
        if (dyn->isDirty()) {
            list << dyn;
        }
    }
    return list;
}

/** \brief Return true if there are modifications to save in the rights. */
bool tkUser::hasModifiedRightsToStore() const
{
    return d->m_ModifiedRoles.count();
}

/** \brief Return the list of modified rights roles. */
QStringList tkUser::modifiedRoles() const
{
    return d->m_ModifiedRoles.toList();
}

// HEADER/FOOTER/WATERMARK MANAGEMENT
/**
  \brief Defines the header, footer and watermark (exactly in this order) to use for generic printing.
  \sa tkTextDocumentExtra, tkTextDocumentExtra::toXml()
*/
void tkUser::setExtraDocument(tkTextDocumentExtra *extra, const int index )
{
//    qWarning() << "setExtraDocument" << d->documentIndexToName(index);
//    if (d->m_Docs.contains(index)) {
//        delete d->m_Docs[index];
//        d->m_Docs.remove(index);
//    }
//    d->m_Docs.insert(index, extra);
    // TODO save to dynamicdatas
//    addDynamicDataFromDatabase();
}

void tkUser::setExtraDocumentHtml(const QVariant &val, const int index )
{
//    if (d->m_Docs.contains(index))
//       d->m_Docs[index]->setHtml(val.toString());
//    else {
//        d->createExtraDoc(d->documentIndexToName(index).toAscii(), val, true);
//    }
}

tkTextDocumentExtra *tkUser::extraDocument(const int index) const
{
//    qWarning() << "extraDoc" << d->m_Docs.keys();
//    Q_ASSERT(d->m_Docs.contains(index));
//    if (d->m_Docs.contains(index))
//       return d->m_Docs.value(index);
//    return 0;
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Viewers -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief For debugging purpose only */
void tkUser::warn() const
{
    if (!tkGlobal::isDebugCompilation())
        return;
    foreach(const QString &s, warnText())
        tkLog::addMessage("tkUser", s);
}

QStringList tkUser::warnText() const
{
    if (!tkGlobal::isDebugCompilation())
        return QStringList();

    QStringList list;
    int i;
    tkUserBase * tkb = tkUserBase::instance();

    QString tmp;
    for (i = 0; i < USER_MaxParam; i++ )
        tmp = QString("%1 = %2\n" )
        .arg(tkb->field(Table_USERS , i))
        .arg(d->m_Table_Field_Value.value(Table_USERS ).value(i ).toString() );
    list << tmp;
    tmp.clear();

    const QList<UserDynamicData*> &dynList = modifiedDynamicDatas();

    if (d->m_DynamicDatas.count() == 0 )
        list << "    /!\\ NO DATAS RECORDED /!\\ ";
    else
        foreach(const UserDynamicData *dyn, d->m_DynamicDatas) {
            tmp += "\n\nDATA : " + dyn->name() + "\n";
            tmp += QString("%1\n" ).arg(dyn->warnText());
        }
    list << tmp;
    tmp.clear();

    if (d->m_Role_Rights.count() == 0 )
        list <<  "    /!\\ NO RIGHTS RECORDED /!\\ " ;
    else
        foreach(const QString & id, d->m_Role_Rights.keys()){
            tmp += "\n\nRIGHT : " + id + "\n";
            for (i = 0; i < RIGHTS_MaxParam; i++ )
                tmp += QString("%1 : %2 = %3\n" )
                                   .arg(id )
                                   .arg(tkb->field(Table_RIGHTS , i))
                                   .arg(d->m_Role_Rights.value(id ).value(i ).toString() );
        }
    list << tmp;
    tmp.clear();

    tmp += "  * Values *\n";
    tmp += QString("%1 = %2\n" ).arg("m_Modifiable" ).arg(isModifiable() );
    tmp += QString("%1 = %2\n" ).arg("m_Modified" ).arg(isModified() );
    tmp += QString("%1 = %2\n" ).arg("m_IsNull" ).arg(d->m_IsNull );
    tmp += QString("%1 = %2\n" ).arg("hasModifiedDynamicDatas" ).arg(hasModifiedDynamicDatasToStore() );
    tmp += "modifiedDynamicDatas = ";
    foreach( UserDynamicData *dyn, dynList)
             tmp += dyn->name() + "; ";
    tmp.chop(2);
    tmp += "\n";
    list << tmp;
    return list;
}

