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
/**
  \class Patients::PatientModel
  \brief All Patients' accessible data are provided by this model.
  The model creates and manages the Core::IPatient model wrapper.
  \sa Core::IMode, Core::IPatient
*/

#include "patientmodel.h"
#include "patientbase.h"
#include "constants_db.h"
#include "constants_settings.h"
#include "constants_trans.h"

#include <usermanagerplugin/usermodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

#include <medicalutils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QObject>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QPixmap>
#include <QBuffer>
#include <QByteArray>
#include <QUuid>

using namespace Patients;
using namespace Trans::ConstantTranslations;

/** The PatientModelWrapper is installed in Core::ICore by MainWindow */


static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }


namespace Patients {
namespace Internal {


class PatientModelPrivate
{
public:
    PatientModelPrivate(PatientModel *parent) :
            m_SqlPatient(0),
            m_SqlPhoto(0),
            q(parent)
    {
        m_UserUuid = userModel()->currentUserData(Core::IUser::Uuid).toString();
        q->connect(userModel(), SIGNAL(userConnected(QString)), q, SLOT(changeUserUuid(QString)));

        // install the Core Patient wrapper
//        Core::ICore::instance()->setPatient(q);
    }

    ~PatientModelPrivate ()
    {
        if (m_SqlPatient) {
            delete m_SqlPatient;
            m_SqlPatient = 0;
        }
        if (m_SqlPhoto) {
            delete m_SqlPhoto;
            m_SqlPhoto = 0;
        }
//        if (m_PatientWrapper) {
//            delete m_PatientWrapper;
//            m_PatientWrapper = 0;
//        }
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
        /** \todo filter photo SQL as well */

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

        filter += QString(" ORDER BY `%1` ASC").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_NAME));

//        qWarning() << filter;

        m_SqlPatient->setFilter(filter);
        m_SqlPatient->select();

//        qWarning() << m_SqlPatient->query().lastQuery();

        q->reset();
    }

    QIcon iconizedGender(const QModelIndex &index)
    {
        const QString &g = m_SqlPatient->data(m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER)).toString();
        if (g=="M") {
            return theme()->icon(Core::Constants::ICONMALE);
        } else if (g=="F") {
            return theme()->icon(Core::Constants::ICONFEMALE);
        } else if (g=="H") {
            return theme()->icon(Core::Constants::ICONHERMAPHRODISM);
        }
        return QIcon();
    }

    bool savePatientPhoto(const QPixmap &pix, const QString &patientUid)
    {
        if (pix.isNull() || patientUid.isEmpty())
            return false;

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        pix.save(&buffer, "PNG"); // writes image into ba in PNG format {6a247e73-c241-4556-8dc8-c5d532b8457e}

        // need creation or update ?
        QHash<int, QString> where;
        where.insert(Constants::PHOTO_PATIENT_UID, QString("='%1'").arg(patientUid));
        bool create = patientBase()->count(Constants::Table_PATIENT_PHOTO, Constants::PHOTO_PATIENT_UID, patientBase()->getWhereClause(Constants::Table_PATIENT_PHOTO, where)) == 0;

        QSqlQuery query(patientBase()->database());
        QString req;
        if (create) {
            req = patientBase()->prepareInsertQuery(Constants::Table_PATIENT_PHOTO);
            query.prepare(req);
            query.bindValue(Constants::PHOTO_ID, QVariant());
            query.bindValue(Constants::PHOTO_UID, patientUid);
            query.bindValue(Constants::PHOTO_PATIENT_UID, patientUid);
            query.bindValue(Constants::PHOTO_BLOB, ba);
        } else {
            req = patientBase()->prepareUpdateQuery(Constants::Table_PATIENT_PHOTO, Constants::PHOTO_BLOB, where);
            query.prepare(req);
            query.bindValue(Constants::PHOTO_BLOB, ba);
        }

        query.exec();
        if (!query.isActive()) {
            Utils::Log::addQueryError(q, query);
            return false;
        }
        return true;
    }

    QPixmap getPatientPhoto(const QString &patientUid)
    {
        QHash<int, QString> where;
        where.insert(Constants::PHOTO_PATIENT_UID, QString("='%1'").arg(patientUid));
        if (patientBase()->count(Constants::Table_PATIENT_PHOTO, Constants::PHOTO_PATIENT_UID, patientBase()->getWhereClause(Constants::Table_PATIENT_PHOTO, where)) == 0)
            return QPixmap();

        QSqlQuery query(patientBase()->database());
        QString req = patientBase()->select(Constants::Table_PATIENT_PHOTO, Constants::PHOTO_BLOB, where);
        if (!query.exec(req)) {
            Utils::Log::addQueryError(q, query);
            return QPixmap();
        } else {
            if (query.next()) {
                QPixmap pix;
                pix.loadFromData(query.value(0).toByteArray());
                return pix;
            }
        }
        return QPixmap();
    }


public:
    QSqlTableModel *m_SqlPatient, *m_SqlPhoto;
//    PatientModelWrapper *m_PatientWrapper;
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

    d->m_SqlPhoto = new QSqlTableModel(this, patientBase()->database());
    d->m_SqlPhoto->setTable(patientBase()->table(Constants::Table_PATIENT_PHOTO));

    d->connectSqlPatientSignals();
    changeUserUuid(d->m_UserUuid);
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
    QList<int> ids = userModel()->practionnerLkIds(uuid);
    d->m_LkIds.clear();
    foreach(int i, ids)
        d->m_LkIds.append(QString::number(i) + ",");
    d->m_LkIds.chop(1);
    d->refreshFilter();
}

int PatientModel::rowCount(const QModelIndex &) const
{
    return d->m_SqlPatient->rowCount();
}

int PatientModel::columnCount(const QModelIndex &) const
{
    return Core::IPatient::NumberOfColumns;
}

int PatientModel::numberOfFilteredPatients() const
{
    return patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_NAME, d->m_SqlPatient->filter());
}

bool PatientModel::hasChildren(const QModelIndex &) const
{
    return false;
}

QVariant PatientModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::ToolTipRole || role==Qt::EditRole) {
        using namespace Core;
        int col = -1;
        switch (index.column()) {
        case IPatient::UsersUidList:  break;
        case IPatient::GroupsUidList: break;
        case IPatient::Id :           col = Constants::IDENTITY_ID;         break;
        case IPatient::Uid:           col = Constants::IDENTITY_UID;        break;
        case IPatient::FamilyUid:     col = Constants::IDENTITY_FAMILY_UID; break;
        case IPatient::BirthName:     col = Constants::IDENTITY_NAME;       break;
        case IPatient::SecondName:    col = Constants::IDENTITY_SECONDNAME;        break;
        case IPatient::Firstname:     col = Constants::IDENTITY_FIRSTNAME;           break;
        case IPatient::Gender:        col = Constants::IDENTITY_GENDER;            break;
        case IPatient::GenderIndex:
            {
                const QString &g = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER).data().toString();
                if (g=="M")
                    return 0;
                if (g=="F")
                    return 1;
                if (g=="H")
                    return 2;
                return -1;
            }
        case IPatient::DateOfBirth:   col = Constants::IDENTITY_DOB;               break;
        case IPatient::MaritalStatus: col = Constants::IDENTITY_MARITAL_STATUS;    break;
        case IPatient::DateOfDeath:   col = Constants::IDENTITY_DATEOFDEATH;       break;
        case IPatient::Profession:    col = Constants::IDENTITY_PROFESSION;        break;
        case IPatient::Street:        col = Constants::IDENTITY_ADDRESS_STREET;    break;
        case IPatient::ZipCode:       col = Constants::IDENTITY_ADDRESS_ZIPCODE;   break;
        case IPatient::City:          col = Constants::IDENTITY_ADRESS_CITY;       break;
        case IPatient::Country:       col = Constants::IDENTITY_ADDRESS_COUNTRY;   break;
        case IPatient::AddressNote:   col = Constants::IDENTITY_ADDRESS_NOTE;      break;
        case IPatient::Mails:         col = Constants::IDENTITY_MAILS;             break;
        case IPatient::Tels:          col = Constants::IDENTITY_TELS;              break;
        case IPatient::Faxes:         col = Constants::IDENTITY_FAXES;             break;
        case IPatient::TitleIndex :   col = Constants::IDENTITY_TITLE;            break;
        case IPatient::Title:
            {
                col = Constants::IDENTITY_TITLE;
                int t = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), col)).toInt();
                switch (t) {
                case Trans::Constants::Mister:    return tkTr(Trans::Constants::MISTER);
                case Trans::Constants::Miss :     return tkTr(Trans::Constants::MISS);
                case Trans::Constants::Madam :    return tkTr(Trans::Constants::MADAM);
                case Trans::Constants::Doctor :   return tkTr(Trans::Constants::DOCTOR);
                case Trans::Constants::Professor: return tkTr(Trans::Constants::PROFESSOR);
                case Trans::Constants::Captain :  return tkTr(Trans::Constants::CAPTAIN);
                default :       return QString();
                }
                return QString();
            }
        case IPatient::FullName:
            {
                const QString &name = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_NAME)).toString();
                const QString &sec = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_SECONDNAME)).toString();
                const QString &sur = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_FIRSTNAME)).toString();
                if (!sec.isEmpty()) {
                    return QString("%1 - %2 %3").arg(name, sec, sur);
                } else {
                    return QString("%1 %2").arg(name, sur);
                }
                break;
            }
        case IPatient::FullAddress:
            {
                const QString &street = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_STREET)).toString();
                const QString &city = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADRESS_CITY)).toString();
                const QString &zip = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_ZIPCODE)).toString();
                return QString("%1 %2 %3").arg(street, city, zip);
            }
        case IPatient::Age:
            {
                const QDate &dob = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_DOB)).toDate();
                return MedicalUtils::readableAge(dob);
            }
        case IPatient::IconizedGender: return d->iconizedGender(index);
        case IPatient::Photo :
            {
                QString patientUid = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_UID).data().toString();
                return d->getPatientPhoto(patientUid);
            }
        case IPatient::PractitionnerLkID: return d->m_LkIds;
        }
        return d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), col), role);
    }
    else if (role==Qt::DecorationRole) {
        switch (index.column()) {
        case Core::IPatient::IconizedGender: return d->iconizedGender(index);
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

//    qWarning() << index << value << role << Qt::EditRole;

    if (role == Qt::EditRole) {
        using namespace Core;
        int col = -1;
        switch (index.column()) {
        case IPatient::UsersUidList:  break;
        case IPatient::GroupsUidList: break;
        case IPatient::Id :           col = Constants::IDENTITY_ID;               break;
        case IPatient::Uid:           col = Constants::IDENTITY_UID;              break;
        case IPatient::FamilyUid:     col = Constants::IDENTITY_FAMILY_UID;       break;
        case IPatient::BirthName:     col = Constants::IDENTITY_NAME;             break;
        case IPatient::SecondName:    col = Constants::IDENTITY_SECONDNAME;       break;
        case IPatient::Firstname:     col = Constants::IDENTITY_FIRSTNAME;          break;
        case IPatient::GenderIndex:
            {
                col = Constants::IDENTITY_GENDER;
                QString g;
                switch (value.toInt())
                {
                case 0: g = "M"; break;
                case 1: g = "F"; break;
                case 2: g = "H"; break;
                }
                return d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER), g, role);
                break;
            }
        case IPatient::Gender:
            {
                const QString &g = value.toString();
                QString toSave;
                switch (genders().indexOf(g)) {
                case 0 : toSave = "M"; break;
                case 1 : toSave = "F"; break;
                case 2:  toSave = "H"; break;
                default: Utils::Log::addError(this, "Unknown gender " + g);
                }
                return d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER), toSave, role);
                break;
            }
        case IPatient::DateOfBirth:   col = Constants::IDENTITY_DOB;              break;
        case IPatient::MaritalStatus: col = Constants::IDENTITY_MARITAL_STATUS;   break;
        case IPatient::DateOfDeath:   col = Constants::IDENTITY_DATEOFDEATH;      break;
        case IPatient::Profession:    col = Constants::IDENTITY_PROFESSION;       break;
        case IPatient::Street:        col = Constants::IDENTITY_ADDRESS_STREET;   break;
        case IPatient::ZipCode:       col = Constants::IDENTITY_ADDRESS_ZIPCODE;  break;
        case IPatient::City:          col = Constants::IDENTITY_ADRESS_CITY;      break;
        case IPatient::Country:       col = Constants::IDENTITY_ADDRESS_COUNTRY;  break;
        case IPatient::AddressNote:   col = Constants::IDENTITY_ADDRESS_NOTE;     break;
        case IPatient::Mails:         col = Constants::IDENTITY_MAILS;            break;
        case IPatient::Tels:          col = Constants::IDENTITY_TELS;             break;
        case IPatient::Faxes:         col = Constants::IDENTITY_FAXES;            break;
        case IPatient::Title :
            {
                QString t = value.toString();
                int id = -1;
                col = Constants::IDENTITY_TITLE;
                if (t == tkTr(Trans::Constants::MISTER)) {
                    id = Trans::Constants::Mister;
                } else if (t == tkTr(Trans::Constants::MISS)) {
                    id = Trans::Constants::Miss;
                } else if (t == tkTr(Trans::Constants::MADAM)) {
                    id = Trans::Constants::Madam;
                } else if (t == tkTr(Trans::Constants::DOCTOR)) {
                    id = Trans::Constants::Doctor;
                } else if (t == tkTr(Trans::Constants::PROFESSOR)) {
                    id = Trans::Constants::Professor;
                } else if (t == tkTr(Trans::Constants::CAPTAIN)) {
                    id = Trans::Constants::Captain;
                }
                if (id != -1) {
                    return d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), col), id, role);
                }
            }
        case IPatient::FullName:
            {
                return true;
            }
        case IPatient::Photo:
            {
                QPixmap pix = value.value<QPixmap>();
                QString patientUid = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_UID).data().toString();
                return d->savePatientPhoto(pix, patientUid);
            }
        }
        if (col != -1) {
            bool ok = d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), col), value, role);
            if (!ok)
                Utils::Log::addQueryError(this, d->m_SqlPatient->query());
//            qWarning() << d->m_SqlPatient->index(index.row(), col).data() << d->m_SqlPatient->index(index.row(), Constants::IDENTITY_UID).data();
            return ok;
        }
    }
    return true;
}

void PatientModel::setFilter(const QString &name, const QString &firstname, const QString &uuid, const FilterOn on)
{
    // Calculate new filter
    switch (on) {
    case FilterOnFullName :
        {
            // WHERE (NAME || SECONDNAME || SURNAME LIKE '%') OR (NAME LIKE '%')
            const QString &nameField = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_NAME);
            const QString &secondField = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_SECONDNAME);
            const QString &surField = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_FIRSTNAME);
            d->m_ExtraFilter.clear();
//            d->m_ExtraFilter =  name + " || ";
//            d->m_ExtraFilter += second + " || ";
//            d->m_ExtraFilter += sur + " ";
            d->m_ExtraFilter += QString("((%1 LIKE '%2%' ").arg(nameField, name);
            d->m_ExtraFilter += QString("OR %1 LIKE '%2%') ").arg(secondField, name);
            if (!firstname.isEmpty())
                d->m_ExtraFilter += QString("AND %1 LIKE '%2%')").arg(surField, firstname);
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
            d->m_ExtraFilter = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_NAME) + " ";
            d->m_ExtraFilter += QString("LIKE '%%1%'").arg(name);
            break;
        }
    case FilterOnCity:
        {
//            // WHERE CITY LIKE '%'
//            d->m_ExtraFilter.clear();
//            d->m_ExtraFilter = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_ADRESS_CITY) + " ";
//            d->m_ExtraFilter += QString("LIKE '%%1%'").arg(filter);
            break;
        }
    case FilterOnUuid:
        {
            // WHERE PATIENT_UID='xxxx'
            d->m_ExtraFilter.clear();
            d->m_ExtraFilter = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_UID) + " ";
            d->m_ExtraFilter += QString("='%1'").arg(uuid);
            break;
        }
    }

    d->refreshFilter();
}

QString PatientModel::filter() const
{
    if (d->m_SqlPatient)
        return d->m_SqlPatient->filter();
    return QString();
}

//QVariant PatientModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    return QVariant();
//}

bool PatientModel::insertRows(int row, int count, const QModelIndex &parent)
{
//    qWarning() << "PatientModel::insertRows" << row << count << parent;
    bool ok = true;
    beginInsertRows(parent, row, row+count);
    for(int i=0; i < count; ++i) {
        if (!d->m_SqlPatient->insertRow(row+i, parent)) {
            ok = false;
            Utils::Log::addError(this, "Unable to create a new patient. PatientModel::insertRows()");
            continue;
        }
        // find an unused uuid
        bool findUuid = false;
        QString uuid;
        while (!findUuid) {
            /** \todo Take care to inifinite looping... */
            uuid = QUuid::createUuid().toString();
            QString f = QString("%1='%2'").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_UID), uuid);
            findUuid = (patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_UID, f) == 0);
        }
        if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_UID), uuid)) {
            ok = false;
            Utils::Log::addError(this, "Unable to setData to newly created patient.", __FILE__,__LINE__);
        }
        qWarning() << ",nnnnnnnnnnnn" << user()->value(Core::IUser::PersonalLinkId);
        if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_LK_TOPRACT_LKID), user()->value(Core::IUser::PersonalLinkId))) { // linkIds
            ok = false;
            Utils::Log::addError(this, "Unable to setData to newly created patient.", __FILE__,__LINE__);
        }
    }
    endInsertRows();
    return ok;
}

bool PatientModel::removeRows(int row, int count, const QModelIndex &parent)
{
//    qWarning() << "PatientModel::removeRows" << row << count << parent;
    return true;
}

void PatientModel::fetchMore(const QModelIndex &parent)
{
    if (d->m_SqlPatient)
        d->m_SqlPatient->fetchMore(parent);
}

bool PatientModel::canFetchMore(const QModelIndex &parent) const
{
    if (d->m_SqlPatient)
        return d->m_SqlPatient->canFetchMore(parent);
    return false;
}

bool PatientModel::submit()
{
    bool ok = true;
    if (!d->m_SqlPatient->submitAll())
        ok = false;
    return true;
}

bool PatientModel::refreshModel()
{
    d->refreshFilter();
    d->m_SqlPatient->select();
    reset();
    return true;
}

/** \brief Return the list patient name corresponding to the uuids list passed as param. Static.*/
QList<QString> PatientModel::patientName(const QList<QString> &uuids)
{
    QList<QString> names;
    if (!patientBase()->database().transaction())
        Utils::Log::addError("PatientModel", "Unable to set transaction with patient database");
    foreach(const QString &u, uuids) {
        QSqlQuery query(patientBase()->database());
        QHash<int, QString> where;
        where.insert(Constants::IDENTITY_UID, QString("='%1'").arg(u));
        QString req = patientBase()->select(Constants::Table_IDENT, QList<int>() << Constants::IDENTITY_NAME << Constants::IDENTITY_SECONDNAME << Constants::IDENTITY_FIRSTNAME, where);
        if (query.exec(req)) {
            if (query.next()) {
                if (!query.value(1).toString().isEmpty())
                    names << QString("%1 - %2 %3").arg(query.value(0).toString(), query.value(1).toString(), query.value(2).toString());
                else
                    names << QString("%1 %3").arg(query.value(0).toString(), query.value(2).toString());
            }
        } else {
            Utils::Log::addQueryError("PatientModel", query);
        }
    }
    patientBase()->database().commit();
    return names;
}
