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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "patientmodel.h"
#include "patientbase.h"
#include "constants_db.h"
#include "constants_settings.h"
#include "constants_trans.h"

#include <usermanagerplugin/usermodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <medicalutils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QObject>
#include <QSqlTableModel>
#include <QSqlDatabase>

using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }


namespace Patients {
namespace Internal {
class PatientModelPrivate
{
public:
    PatientModelPrivate(PatientModel *parent) :
            m_SqlPatient(0), q(parent)
    {
        m_UserUuid = userModel()->currentUserData(UserPlugin::User::Uuid).toString();
        q->connect(userModel(), SIGNAL(userConnected(QString)), q, SLOT(changeUserUuid(QString)));
    }


    ~PatientModelPrivate ()
    {
        if (m_SqlPatient) {
            delete m_SqlPatient;
            m_SqlPatient = 0;
        }
    }

    void connectSqlPatientSignals()
    {
        q->connect(m_SqlPatient, SIGNAL(layoutAboutToBeChanged()), q, SIGNAL(layoutAboutToBeChanged()));
        q->connect(m_SqlPatient, SIGNAL(layoutChanged()), q, SIGNAL(layoutChanged()));
        q->connect(m_SqlPatient, SIGNAL(columnsAboutToBeInserted(QModelIndex, int , int )), q, SIGNAL(columnsAboutToBeInserted(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(rowsAboutToBeInserted(QModelIndex, int , int )), q, SIGNAL(rowsAboutToBeInserted(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(rowsInserted(QModelIndex, int , int )), q, SIGNAL(rowsInserted(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int , int )), q, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(rowsRemoved(QModelIndex, int , int )), q, SIGNAL(rowsRemoved(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(columnsAboutToBeInserted(QModelIndex, int , int )), q, SIGNAL(columnsAboutToBeInserted(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(columnsInserted(QModelIndex, int , int )), q, SIGNAL(columnsInserted(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int , int )), q, SIGNAL( columnsAboutToBeRemoved(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(columnsRemoved(QModelIndex, int , int )), q, SIGNAL(columnsRemoved(QModelIndex, int , int )));
        q->connect(m_SqlPatient, SIGNAL(modelAboutToBeReset()), q, SIGNAL(modelAboutToBeReset()));
        q->connect(m_SqlPatient, SIGNAL(modelReset()), q, SIGNAL(modelReset()));
        q->connect(m_SqlPatient, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int , QModelIndex, int)), q, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int , QModelIndex, int)));
        q->connect(m_SqlPatient, SIGNAL(rowsMoved(QModelIndex, int , int , QModelIndex, int )), q, SIGNAL(rowsMoved(QModelIndex, int , int , QModelIndex, int )));
        q->connect(m_SqlPatient, SIGNAL(columnsAboutToBeMoved(QModelIndex, int , int , QModelIndex, int )), q, SIGNAL(columnsAboutToBeMoved(QModelIndex, int , int , QModelIndex, int )));
        q->connect(m_SqlPatient, SIGNAL(columnsMoved(QModelIndex, int , int , QModelIndex, int )), q, SIGNAL(columnsMoved(QModelIndex, int , int , QModelIndex, int )));
    }

    void refreshFilter()
    {
        // WHERE (LK_ID IN (SELECT LK_ID FROM LK_TOPRACT WHERE LK_PRACT_UID='...')) OR
        //       (LK_ID IN (SELECT LK_ID FROM LK_TOPRACT WHERE LK_GROUP_UID='...'))

        // Manage virtual patients
        QHash<int, QString> where;
        if (!settings()->value(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS).toBool())
            where.insert(Constants::IDENTITY_ISVIRTUAL, "=0");

        // Manage User Link ID
        where.insert(Constants::IDENTITY_LK_TOPRACT_LKID, QString("IN (%1)").arg(m_LkIds));
        QString filter = patientBase()->getWhereClause(Constants::Table_IDENT, where);

        if (!m_ExtraFilter.isEmpty())
            filter += QString(" AND (%1)").arg(m_ExtraFilter);

        filter += QString(" ORDER BY `%1` ASC").arg(patientBase()->field(Constants::Table_IDENT, Constants::IDENTITY_NAME));

//        qWarning() << filter;

        m_SqlPatient->setFilter(filter);
        m_SqlPatient->select();

//        qWarning() << m_SqlPatient->query().lastQuery();

        q->reset();
    }

    void retreivePractionnerLkIds()
    {
        // Get Lk_Ids for this practitionner
        /** \todo manage user's groups */
        QHash<int, QString> where;
        where.clear();
        where.insert(Constants::LK_TOPRACT_PRACT_UUID, QString("='%1'").arg(m_UserUuid));
        QString req = patientBase()->select(Constants::Table_LK_TOPRACT, Constants::LK_TOPRACT_LKID, where);
        QSqlQuery query(req, m_SqlPatient->database());
        if (query.isActive()) {
            while (query.next())
                m_LkIds.append(query.value(0).toString() + ",");
            m_LkIds.chop(1);
        } else {
            Utils::Log::addQueryError(q, query);
        }
    }

public:
    QSqlTableModel *m_SqlPatient;
    QString m_ExtraFilter;
    QString m_LkIds;
    QString m_UserUuid;

private:
    PatientModel *q;

};
}
}

PatientModel *PatientModel::m_ActiveModel = 0;

static const QColor femaleColor(255, 207, 255, 90); // ffcfff
static const QColor maleColor(225, 225, 255, 90);    //e0e0ff
static const QColor hermaColor(255, 214, 255, 90);   //ffd6ff

PatientModel::PatientModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::PatientModelPrivate(this))
{
    setObjectName("PatientModel");
    d->m_SqlPatient = new QSqlTableModel(this, patientBase()->database());
    d->m_SqlPatient->setTable(patientBase()->table(Constants::Table_IDENT));
    d->connectSqlPatientSignals();
    d->retreivePractionnerLkIds();
    d->refreshFilter();
//    d->m_SqlPatient->select();
}

PatientModel::~PatientModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

void PatientModel::changeUserUuid(const QString &uuid)
{
    d->m_UserUuid = uuid;
    d->retreivePractionnerLkIds();
    d->refreshFilter();
}

int PatientModel::rowCount(const QModelIndex &parent) const
{
    return d->m_SqlPatient->rowCount();
}

int PatientModel::columnCount(const QModelIndex &parent) const
{
    return NumberOfColumns;
}

int PatientModel::numberOfFilteredPatients() const
{
    return patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_NAME, d->m_SqlPatient->filter());
}

bool PatientModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

QVariant PatientModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::ToolTipRole || role==Qt::EditRole) {
        int col = -1;
        switch (index.column()) {
        case UsersUidList:  break;
        case GroupsUidList: break;
        case Id :           col = Constants::IDENTITY_ID;         break;
        case Uid:           col = Constants::IDENTITY_UID;        break;
        case FamilyUid:     col = Constants::IDENTITY_FAMILY_UID; break;
        case BirthName:     col = Constants::IDENTITY_NAME;       break;
        case SecondName:    col = Constants::IDENTITY_SECONDNAME;       break;
        case Surname:       col = Constants::IDENTITY_SURNAME;       break;
        case Gender:        col = Constants::IDENTITY_GENDER;       break;
        case DateOfBirth:   col = Constants::IDENTITY_DOB;       break;
        case MaritalStatus: col = Constants::IDENTITY_MARITAL_STATUS;       break;
        case DateOfDeath:   col = Constants::IDENTITY_DATEOFDEATH;       break;
        case Street:        col = Constants::IDENTITY_ADDRESS_STREET;       break;
        case ZipCode:       col = Constants::IDENTITY_ADDRESS_ZIPCODE;       break;
        case City:          col = Constants::IDENTITY_ADRESS_CITY;       break;
        case Country:       col = Constants::IDENTITY_ADDRESS_COUNTRY;       break;
        case AddressNote:   col = Constants::IDENTITY_ADDRESS_NOTE;       break;
        case Mails:         col = Constants::IDENTITY_MAILS;       break;
        case Tels:          col = Constants::IDENTITY_TELS;       break;
        case Faxes:         col = Constants::IDENTITY_FAXES;       break;
        case Title :
            {
                col = Constants::IDENTITY_TITLE;
                int t = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_TITLE)).toInt();
                switch (t) {
                case Mister: return qApp->translate(Constants::TRANS_CONTEXT, Constants::MISTER);
                case Miss :  return qApp->translate(Constants::TRANS_CONTEXT, Constants::MISS);
                case Madam :  return qApp->translate(Constants::TRANS_CONTEXT, Constants::MADAM);
                case Doctor :  return qApp->translate(Constants::TRANS_CONTEXT, Constants::DOCTOR);
                case Professor :  return qApp->translate(Constants::TRANS_CONTEXT, Constants::PROFESSOR);
                case Captain :  return qApp->translate(Constants::TRANS_CONTEXT, Constants::CAPTAIN);
                default : return QString();
                }
                return QString();
            }
        case FullName:
            {
                const QString &name = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_NAME)).toString();
                const QString &sec = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_SECONDNAME)).toString();
                const QString &sur = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_SURNAME)).toString();
                if (!sec.isEmpty()) {
                    return QString("%1 - %2 %3").arg(name, sec, sur);
                } else {
                    return QString("%1 %2").arg(name, sur);
                }
            }
        case FullAddress:
            {
                const QString &street = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_STREET)).toString();
                const QString &city = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADRESS_CITY)).toString();
                const QString &zip = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_ZIPCODE)).toString();
                return QString("%1 %2 %3").arg(street, city, zip);
            }
        case Age:
            {
                const QDate &dob = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_DOB)).toDate();
                return MedicalUtils::readableAge(dob);
            }
        }
        return d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), col), role);
    }
    else if (role==Qt::DecorationRole) {
        switch (index.column()) {
        case IconizedGender:
            const QString &g = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER)).toString();
            if (g=="M") {
                return theme()->icon(Core::Constants::ICONMALE);
            } else if (g=="F") {
                return theme()->icon(Core::Constants::ICONFEMALE);
            } else if (g=="H") {
                return theme()->icon(Core::Constants::ICONHERMAPHRODISM);
            }
            return g;
        }
    } else if (role==Qt::BackgroundRole) {
        if (settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool()) {
            const QString &g = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER)).toString();
            if (g=="M") {
                return maleColor;
            } else if (g=="F") {
                return femaleColor;
            } else if (g=="H") {
                return hermaColor;
            }
        }
    }
    return QVariant();
}

bool PatientModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {
        int col = -1;
        switch (index.column()) {
        case UsersUidList:  break;
        case GroupsUidList: break;
        case Id :           col = Constants::IDENTITY_ID;         break;
        case Uid:           col = Constants::IDENTITY_UID;        break;
        case FamilyUid:     col = Constants::IDENTITY_FAMILY_UID; break;
        case BirthName:     col = Constants::IDENTITY_NAME;       break;
        case SecondName:    col = Constants::IDENTITY_SECONDNAME;       break;
        case Surname:       col = Constants::IDENTITY_SURNAME;       break;
        case Gender:        col = Constants::IDENTITY_GENDER;       break;
        case DateOfBirth:   col = Constants::IDENTITY_DOB;       break;
        case MaritalStatus: col = Constants::IDENTITY_MARITAL_STATUS;       break;
        case DateOfDeath:   col = Constants::IDENTITY_DATEOFDEATH;       break;
        case Street:        col = Constants::IDENTITY_ADDRESS_STREET;       break;
        case ZipCode:       col = Constants::IDENTITY_ADDRESS_ZIPCODE;       break;
        case City:          col = Constants::IDENTITY_ADRESS_CITY;       break;
        case Country:       col = Constants::IDENTITY_ADDRESS_COUNTRY;       break;
        case AddressNote:   col = Constants::IDENTITY_ADDRESS_NOTE;       break;
        case Mails:         col = Constants::IDENTITY_MAILS;       break;
        case Tels:          col = Constants::IDENTITY_TELS;       break;
        case Faxes:         col = Constants::IDENTITY_FAXES;       break;
        case Title :
            {
                QString t = value.toString();
                int id = -1;
                col = Constants::IDENTITY_TITLE;
                if (t == qApp->translate(Constants::TRANS_CONTEXT, Constants::MISTER)) {
                    id = Mister;
                } else if (t == qApp->translate(Constants::TRANS_CONTEXT, Constants::MISS)) {
                    id = Miss;
                } else if (t == qApp->translate(Constants::TRANS_CONTEXT, Constants::MADAM)) {
                    id = Madam;
                } else if (t == qApp->translate(Constants::TRANS_CONTEXT, Constants::DOCTOR)) {
                    id = Doctor;
                } else if (t == qApp->translate(Constants::TRANS_CONTEXT, Constants::PROFESSOR)) {
                    id = Professor;
                } else if (t == qApp->translate(Constants::TRANS_CONTEXT, Constants::CAPTAIN)) {
                    id = Captain;
                }
                if (id != -1) {
                    return d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), col), id, role);
                }
            }
        case FullName:
            {
                break;
            }
        }
        if (col != -1) {
            return d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), col), value, role);
        }
    }
    return true;
}

void PatientModel::setFilter(const QString &name, const QString &surname, const FilterOn on)
{
    // Calculate new filter
    switch (on) {
    case FilterOnFullName :
        {
            // WHERE (NAME || SECONDNAME || SURNAME LIKE '%') OR (NAME LIKE '%')
            const QString &nameField = patientBase()->field(Constants::Table_IDENT, Constants::IDENTITY_NAME);
            const QString &secondField = patientBase()->field(Constants::Table_IDENT, Constants::IDENTITY_SECONDNAME);
            const QString &surField = patientBase()->field(Constants::Table_IDENT, Constants::IDENTITY_SURNAME);
            d->m_ExtraFilter.clear();
//            d->m_ExtraFilter =  name + " || ";
//            d->m_ExtraFilter += second + " || ";
//            d->m_ExtraFilter += sur + " ";
            d->m_ExtraFilter += QString("((%1 LIKE '%2%' ").arg(nameField, name);
            d->m_ExtraFilter += QString("OR %1 LIKE '%2%') ").arg(secondField, name);
            if (!surname.isEmpty())
                d->m_ExtraFilter += QString("AND %1 LIKE '%2%')").arg(surField, surname);
            else
                d->m_ExtraFilter += ")";

            // OR ...
//            QStringList splitters;
//            if (filter.contains(" ")) {
//                splitters << " ";
//            }
//            if (filter.contains(";")) {
//                splitters << ";";
//            }
//            if (filter.contains(",")) {
//                splitters << ",";
//            }
//            foreach(const QString &s, splitters) {
//                foreach(const QString &f, filter.split(s, QString::SkipEmptyParts)) {
//                    QString like = QString(" LIKE '%1%'").arg(f);
//                    d->m_ExtraFilter += QString(" OR (%1)").arg(name + like);
//                    d->m_ExtraFilter += QString(" OR (%1)").arg(second + like);
//                    d->m_ExtraFilter += QString(" OR (%1)").arg(sur + like);
//                }
//            }
            break;
        }
    case FilterOnName :
        {
            // WHERE NAME LIKE '%'
            d->m_ExtraFilter.clear();
            d->m_ExtraFilter = patientBase()->field(Constants::Table_IDENT, Constants::IDENTITY_NAME) + " ";
            d->m_ExtraFilter += QString("LIKE '%%1%'").arg(name);
            break;
        }
    case FilterOnCity:
        {
//            // WHERE CITY LIKE '%'
//            d->m_ExtraFilter.clear();
//            d->m_ExtraFilter = patientBase()->field(Constants::Table_IDENT, Constants::IDENTITY_ADRESS_CITY) + " ";
//            d->m_ExtraFilter += QString("LIKE '%%1%'").arg(filter);
            break;
        }
    }
    d->refreshFilter();
}

QString PatientModel::filter() const
{
}

QVariant PatientModel::headerData(int section, Qt::Orientation orientation, int role) const
{}

bool PatientModel::insertRows(int row, int count, const QModelIndex &parent)
{}

bool PatientModel::removeRows(int row, int count, const QModelIndex &parent)
{}

void PatientModel::fetchMore(const QModelIndex &parent)
{
    if (d->m_SqlPatient)
        d->m_SqlPatient->fetchMore(parent);
}

bool PatientModel::canFetchMore(const QModelIndex &parent) const
{
    if (d->m_SqlPatient)
        return d->m_SqlPatient->canFetchMore(parent);
}

