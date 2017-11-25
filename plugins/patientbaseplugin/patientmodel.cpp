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
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *       Jerome PINGUET <jerome@jerome.cc>                                 *
 ***************************************************************************/
/**
 * \class Patients::PatientModel
 * \brief All Patients' SQL accessible data are provided by this model.
 * \sa Patients::PatientCore, Core::IMode, Core::IPatient
*/

#include "patientmodel.h"
#include "patientcore.h"
#include "patientbase.h"
#include "constants_db.h"
#include "constants_settings.h"
#include "constants_trans.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatientlistener.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <medicalutils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_titles.h>
#include <extensionsystem/pluginmanager.h>

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>

#include <QObject>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QPixmap>
#include <QBuffer>
#include <QByteArray>
#include <QUuid>

enum { WarnDatabaseFilter = false };

using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}
static inline Patients::Internal::PatientBase *patientBase() {return patientCore()->patientBase();}

static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

namespace Patients {
namespace Internal {
class PatientModelPrivate
{
public:
    PatientModelPrivate(PatientModel *parent) :
        m_SqlPatient(0),
        m_SqlPhoto(0),
        m_EmitCreationAtSubmit(false),
        m_RefreshModelOnCoreDatabaseServerChanged(false),
        q(parent)
    {
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

    void refreshFilter()
    {
        // Manage virtual patients
        QHash<int, QString> where;
        if (!settings()->value(Core::Constants::S_ALLOW_VIRTUAL_DATA, true).toBool())
            where.insert(Constants::IDENTITY_ISVIRTUAL, "=0");

        // All users share the same patients
        //where.insert(Constants::IDENTITY_USER_UUID, QString("IN (%1)").arg(m_LkIds));
        where.insert(Constants::IDENTITY_ISACTIVE, "=1");

        QString filter = patientBase()->getWhereClause(Constants::Table_IDENT, where);

        if (!m_ExtraFilter.isEmpty())
            filter += QString(" AND (%1)").arg(m_ExtraFilter);

        filter += QString(" ORDER BY `%1` ASC")
                .arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_USUALNAME));

        m_SqlPatient->setFilter(filter);

        if (WarnDatabaseFilter)
            LOG_FOR(q, "Filtering patient model with: " + m_SqlPatient->filter());

        m_SqlPatient->select();
    }

    QIcon iconizedGender(const QModelIndex &index)
    {
        //TODO: put this in a separate method/class, there is much duplication of gender (de)referencing in FMF
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
        QSqlDatabase DB = patientBase()->database();
        DB.transaction();
        QSqlQuery query(DB);
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
            query.bindValue(0, ba);
        }
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();
        DB.commit();
        return true;
    }

    QPixmap getPatientPhoto(const QModelIndex &index)
    {
        QHash<int, QString> where;
        QString patientUid = m_SqlPatient->index(index.row(), Constants::IDENTITY_UID).data().toString();

        where.insert(Constants::PHOTO_PATIENT_UID, QString("='%1'").arg(patientUid));
        if (patientBase()->count(Constants::Table_PATIENT_PHOTO, Constants::PHOTO_PATIENT_UID,
                                 patientBase()->getWhereClause(Constants::Table_PATIENT_PHOTO, where)) == 0) {
            return QPixmap();
        }

        QSqlDatabase DB = patientBase()->database();
        DB.transaction();
        QSqlQuery query(DB);
        QString req = patientBase()->select(Constants::Table_PATIENT_PHOTO, Constants::PHOTO_BLOB, where);
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            DB.rollback();
            return QPixmap();
        } else {
            if (query.next()) {
                QPixmap pix;
                pix.loadFromData(query.value(0).toByteArray());
                query.finish();
                DB.commit();
                return pix;
            }
        }
        query.finish();
        DB.commit();
        return QPixmap();
    }
    QString getPatientCreator(const QModelIndex &index)
    {
        QString creator;
        QHash<int, QString> where;
        QString patientUid = m_SqlPatient->index(index.row(), Constants::IDENTITY_UID).data().toString();
        where.insert(Constants::IDENTITY_UID, QString("='%1'").arg(patientUid));
        QSqlDatabase DB = patientBase()->database();
        DB.transaction();
        QSqlQuery query(DB);
        QString req = patientBase()->select(Constants::Table_IDENT, Constants::IDENTITY_USER_UUID, where);
        QString stringUuid;
        if (!query.exec(req)) {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            DB.rollback();
        } else {
            if (query.next()) {
                if (query.value(0).isNull())
                        return QString();
                QByteArray binUuid;
                binUuid = query.value(0).toByteArray();
                query.finish();
                DB.commit();
                stringUuid = QUuid::fromRfc4122(binUuid)
                        .toString()
                        .remove("-")
                        .remove("{")
                        .remove("}");
            }
        }
        query.finish();
        DB.commit();
        QStringList uuids(stringUuid);
        QHash<QString, QString> names = UserPlugin::UserModel::getUserNames(uuids);
        if (!names.isEmpty()) {
            creator = names.value(stringUuid);
        }
        return creator;
    }

public:
    QSqlTableModel *m_SqlPatient, *m_SqlPhoto;
    QString m_ExtraFilter;
    QString m_LkIds;
    QString m_UserUuid;
    QStringList m_CreatedPatientUid;
    bool m_EmitCreationAtSubmit, m_RefreshModelOnCoreDatabaseServerChanged;
    QString m_CurrentPatientUuid;
    QPersistentModelIndex m_CurrentPatientIndex;

private:
    PatientModel *q;
};

} // end namespace Internal
} // end namespace Patients

PatientModel::PatientModel(QObject *parent) :
    QAbstractTableModel(parent), d(new Internal::PatientModelPrivate(this))
{
    setObjectName("PatientModel");
    onCoreDatabaseServerChanged();
    d->m_RefreshModelOnCoreDatabaseServerChanged = true;
    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    //    connect(d->m_SqlPatient, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(onPrimeInsert(int,QSqlRecord&)));
}

PatientModel::~PatientModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

//void PatientModel::onPrimeInsert(int row, QSqlRecord &record)
//{
//    // find an unused uuid
//    bool findUuid = false;
//    QString uuid;
//    while (!findUuid) {
//        // TODO: Take care to inifinite looping...
//        uuid = Utils::Database::createUid();
//        QString f = QString("%1='%2'").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_UID), uuid);
//        findUuid = (patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_UID, f) == 0);
//    }
//    if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_UID), uuid)) {
//        ok = false;
//        LOG_ERROR("Unable to setData to newly created patient.");
//    }
//    if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_LK_TOPRACT_LKID), user()->value(Core::IUser::PersonalLinkId))) { // linkId <- was removed in version 0.11
//        ok = false;
//        LOG_ERROR("Unable to setData to newly created patient.");
//    }
//}

void PatientModel::onCoreDatabaseServerChanged()
{
    // Destroy old model and recreate it
    if (d->m_SqlPatient) {
        disconnect(d->m_SqlPatient);
        delete d->m_SqlPatient;
    }
    d->m_SqlPatient = new QSqlTableModel(this, patientBase()->database());
    d->m_SqlPatient->setTable(patientBase()->table(Constants::Table_IDENT));
    Utils::linkSignalsFromFirstModelToSecondModel(d->m_SqlPatient, this, false);

    if (d->m_SqlPhoto)
        delete d->m_SqlPhoto;
    d->m_SqlPhoto = new QSqlTableModel(this, patientBase()->database());
    d->m_SqlPhoto->setTable(patientBase()->table(Constants::Table_PATIENT_PHOTO));
    if (d->m_RefreshModelOnCoreDatabaseServerChanged)
        d->refreshFilter();
}

/**
 * Call all the plugin Core::IPatientListener::currentPatientAboutToChange()
 * to e.g. save data before changing to the new patient. This method MUST BE
 * used BEFORE any setCurrentPatient(). \n
 * It returns true if you can set the new current patient, otherwise it returns false.
 * You should not set the current patient if this method returns false.
 * \sa setCurrentPatient(), endChangeCurrentPatient(), Core::IPatientListener
 */
bool PatientModel::beginChangeCurrentPatient()
{
    // Call all extensions that provide listeners to patient change: the extensions can now do things like
    // save data BEFORE the patient is changed.
    QList<Core::IPatientListener *> listeners = pluginManager()->getObjects<Core::IPatientListener>();
    for(int i = 0; i < listeners.count(); ++i) {
        if (!listeners.at(i)->currentPatientAboutToChange()) {
            return false;
        }
    }
    return true;
}

/**
  * \brief Sets the index to the given patient QModelIndex.
  *
  * Before changing to the new patient, .
  *
  * Two new signals \e currentPatientChanged() and currentPatientChanged(QModelIndex) are emitted when the new current patient
  * is set. If the new patient is the current one, no signals are emitted.
  *
  * \sa Core::IPatient::currentPatientChanged()
 */
bool PatientModel::setCurrentPatient(const QModelIndex &index)
{
    // Removing current patient?
    if (!index.isValid()) {
        d->m_CurrentPatientIndex = index;
        d->m_CurrentPatientUuid.clear();
        LOG("Removed current patient");
        return true;
    }
    // Same patient as the current one?
    const QString &patientUuid = this->patientUuid(index);
    if (patientUuid.isEmpty()) {
        LOG_ERROR(QString("Empty patient Uuid. Index(%1,%2,%3)").arg(index.row()).arg(index.column()).arg(index.model()->objectName()));
        return false;
    }
    // Already current?
    if (patientUuid == d->m_CurrentPatientUuid)
        return true;
    // Change the internal current patient data
    d->m_CurrentPatientUuid = patientUuid;
    d->m_CurrentPatientIndex = index;
    LOG("setCurrentPatient: " + patientUuid);
    return true;
}

/**
 * After setting the current patient with beginChangeCurrentPatient() and setCurrentPatient(),
 * you must call this method to ensure that all view and submodels get sync with the
 * new current patient. \n
 * Internally, this method emits the two following signals:
 * - currentPatientChanged(QString);
 * - currentPatientChanged(QModelIndex);
 * \sa setCurrentPatient(), beginChangeCurrentPatient(), Core::IPatientListener
 */
void PatientModel::endChangeCurrentPatient()
{
    Q_EMIT currentPatientChanged(d->m_CurrentPatientUuid);
    Q_EMIT currentPatientChanged(d->m_CurrentPatientIndex);
}

/**
 * Return the current patient index in this model.
 * Be warned that after any filter update the index may change.
 */
QModelIndex PatientModel::currentPatient() const
{
    return d->m_CurrentPatientIndex;
}

int PatientModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_SqlPatient->rowCount();
}

int PatientModel::columnCount(const QModelIndex &) const
{
    return Core::IPatient::NumberOfColumns;
}

int PatientModel::numberOfFilteredPatients() const
{
    return patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_USUALNAME, d->m_SqlPatient->filter());
}

bool PatientModel::hasChildren(const QModelIndex &) const
{
    return false;
}

QVariant PatientModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DecorationRole) {
        switch (index.column()) {
        case Core::IPatient::IconizedGender: return d->iconizedGender(index);
        case Core::IPatient::UsualName: return d->iconizedGender(index);
        default: break;
        }
    } else if (role==Qt::DisplayRole || role==Qt::ToolTipRole || role==Qt::EditRole) {
        using namespace Core;
        int col = -1;
        switch (index.column()) {
        case IPatient::UsersUidList:  break;
        case IPatient::GroupsUidList: break;
        case IPatient::Id:            col = Constants::IDENTITY_ID;         break;
        case IPatient::Uid:           col = Constants::IDENTITY_UID;        break;
        case IPatient::FamilyUid:     col = Constants::IDENTITY_FAMILY_UID; break;
        case IPatient::UsualName:     col = Constants::IDENTITY_USUALNAME;  break;
        case IPatient::OtherNames:    col = Constants::IDENTITY_OTHERNAMES; break;
        case IPatient::Firstname:     col = Constants::IDENTITY_FIRSTNAME;  break;
        case IPatient::Gender:        col = Constants::IDENTITY_GENDER;     break;
        case IPatient::GenderIndex:
        {
            // TODO: put this in a separate method/class, there is much duplication of gender (de)referencing
            const QString &g = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER).data().toString();
            if (g=="M")
                return 0;
            if (g=="F")
                return 1;
            if (g=="H")
                return 2;
            return -1;
        }
        case IPatient::DateOfBirth:
        {
            QModelIndex idx = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            QDate dob = d->m_SqlPatient->data(idx).toDate();
            if (role==Qt::DisplayRole) {
                return dob;
            } else if (role==Qt::ToolTipRole) {
                return QString("%1; %2").arg(QLocale().toString(dob, QLocale().dateFormat(QLocale::LongFormat))).arg(MedicalUtils::readableAge(dob, settings()->value(Patients::Constants::S_PEDIATRICSMONTHSYEARSLIMIT, 36).toInt()));
            }
            return dob;
            break;
        }
        case IPatient::MaritalStatus: col = Constants::IDENTITY_MARITAL_STATUS;    break;
        case IPatient::DateOfDeath:   col = Constants::IDENTITY_DATEOFDEATH;       break;
        case IPatient::Profession:    col = Constants::IDENTITY_PROFESSION;        break;
        case IPatient::Street:        col = Constants::IDENTITY_ADDRESS_STREET;    break;
        case IPatient::ZipCode:       col = Constants::IDENTITY_ADDRESS_ZIPCODE;   break;
        case IPatient::City:          col = Constants::IDENTITY_ADDRESS_CITY;      break;
        case IPatient::Country:       col = Constants::IDENTITY_ADDRESS_COUNTRY;   break;
        case IPatient::StateProvince: col = Constants::IDENTITY_ADDRESS_PROVINCE;  break;
        case IPatient::AddressNote:   col = Constants::IDENTITY_ADDRESS_NOTE;      break;
        case IPatient::Email:         col = Constants::IDENTITY_MAILS;             break;
        case IPatient::Tels:          col = Constants::IDENTITY_TELS;              break;
        case IPatient::MobilePhone:   col = Constants::IDENTITY_MOBILE_PHONE;      break;
        case IPatient::WorkPhone:     col = Constants::IDENTITY_WORK_PHONE;        break;
        case IPatient::Faxes:         col = Constants::IDENTITY_FAXES;             break;
        case IPatient::TitleIndex:    col = Constants::IDENTITY_TITLE;             break;
        case IPatient::Title:
        {
            col = Constants::IDENTITY_TITLE;
            int titleIndex = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), col)).toInt();
            switch (titleIndex) {
            case Trans::Constants::Mister:    return tkTr(Trans::Constants::MISTER); break;
            case Trans::Constants::Miss :     return tkTr(Trans::Constants::MISS); break;
            case Trans::Constants::Madam :    return tkTr(Trans::Constants::MADAM); break;
            case Trans::Constants::Doctor :   return tkTr(Trans::Constants::DOCTOR); break;
            case Trans::Constants::Professor: return tkTr(Trans::Constants::PROFESSOR); break;
            case Trans::Constants::Captain :  return tkTr(Trans::Constants::CAPTAIN); break;
            default :       return QString();
            }
            return QString();
        }
        case IPatient::FullName:
        {
            const QString &usualname = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_USUALNAME)).toString();
            const QString &othernames = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_OTHERNAMES)).toString();
            const QString &first = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_FIRSTNAME)).toString();
            QString title;
            // add title
            int titleIndex = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_TITLE)).toInt();
            switch (titleIndex) {
            case Trans::Constants::Mister:    title = tkTr(Trans::Constants::MISTER); break;
            case Trans::Constants::Miss :     title = tkTr(Trans::Constants::MISS); break;
            case Trans::Constants::Madam :    title = tkTr(Trans::Constants::MADAM); break;
            case Trans::Constants::Doctor :   title = tkTr(Trans::Constants::DOCTOR); break;
            case Trans::Constants::Professor: title = tkTr(Trans::Constants::PROFESSOR); break;
            case Trans::Constants::Captain :  title = tkTr(Trans::Constants::CAPTAIN); break;
            }
            if (!title.isEmpty())
                title.append(" ");

            if (!othernames.isEmpty()) {
                return QString("%1 - %2 %3").arg(title+usualname, othernames, first);
            } else {
                return QString("%1 %2").arg(title+usualname, first);
            }
            break;
        }
        case IPatient::FullAddress:
        {
            const QString &street = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_STREET)).toString();
            const QString &province = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_PROVINCE)).toString();
            const QString &city = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_CITY)).toString();
            const QString &zip = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_ZIPCODE)).toString();
            QString country = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_COUNTRY)).toString();
            country = Utils::countryIsoToName(country);
            return QString("%1 %2 %3 %4 %5").arg(street, province, city, zip, country).simplified();
        }
        case IPatient::Age:
        {
            QModelIndex idx = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            QDate dob = d->m_SqlPatient->data(idx).toDate();
            int monthsYearsLimit = settings()->value(Patients::Constants::S_PEDIATRICSMONTHSYEARSLIMIT, 36).toInt();
            int ageInYears = MedicalUtils::ageYears(dob);
            int pediatricsAgeLimit = settings()->value(Patients::Constants::S_PEDIATRICSAGELIMIT, 18).toInt();
            if (ageInYears >= pediatricsAgeLimit) {
                QString ageInYearsString = QString(tr("%n year(s)", "", ageInYears));
                return ageInYearsString;
            }
            return MedicalUtils::readableAge(dob, monthsYearsLimit);
        }
        case IPatient::YearsOld:
        {
            QModelIndex idx = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            return MedicalUtils::ageYears(d->m_SqlPatient->data(idx).toDate());
        }
        case IPatient::IsPediatricToken:
        {
            // returns a space character if true, returns an empty string if false
            QModelIndex idx = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            bool isPediatric = (MedicalUtils::ageYears(d->m_SqlPatient->data(idx).toDate())
                                < settings()->value(Patients::Constants::S_PEDIATRICSAGELIMIT, 18).toInt());
            if (isPediatric)
                return QString(" ");
            return QString();
        }
        case IPatient::IsPediatric:
        {
            QModelIndex idx = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            return (MedicalUtils::ageYears(d->m_SqlPatient->data(idx).toDate())
                    < settings()->value(Patients::Constants::S_PEDIATRICSAGELIMIT, 18).toInt());
        }
        case IPatient::IconizedGender: return d->iconizedGender(index);
        case IPatient::GenderPixmap: return d->iconizedGender(index).pixmap(16,16);
        case IPatient::Photo_32x32 :
        {
            QPixmap pix = d->getPatientPhoto(index);
            if (pix.isNull() || pix.size() == QSize(32,32)) {
                return pix;
            }
            return pix.scaled(QSize(32,32));
        }
        case IPatient::Photo_64x64 :
        {
            QPixmap pix = d->getPatientPhoto(index);
            if (pix.isNull() || pix.size() == QSize(64,64)) {
                return pix;
            }
            return pix.scaled(QSize(64,64));
        }
        case IPatient::PractitionerUuid: return d->getPatientCreator(index);
        }
        QVariant r = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), col), role);
        switch (index.column()) {
        case Core::IPatient::DateOfBirth:
        case Core::IPatient::DateOfDeath:
            return QLocale().toString(r.toDate(), tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL));
        default:
            return r;
        }
    } else if (role==Qt::BackgroundRole) {
        if (settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool()) {
            const QString &g = d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER)).toString();
            if (g=="M") {
                return Constants::maleColor;
            } else if (g=="F") {
                return Constants::femaleColor;
            } else if (g=="H") {
                return Constants::hermaColor;
            }
        }
    }
    return QVariant();
}

bool PatientModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    // Manage IsActive column -> remove the patient from the model.
    if (index.column() == Core::IPatient::IsActive && role==Qt::EditRole) {
        if (!value.toBool()) {
            const QString &uuid = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_UID).data().toString();
            if (!patientCore()->removePatient(uuid))
                return false;
        }
        return true;
    }

    if (role == Qt::EditRole) {
        // TODO: manage patient duplicates when modifying names/uuid
        using namespace Core;
        int col = -1;
        QList<int> colsToEmit;
        colsToEmit << index.column();

        switch (index.column()) {
        case IPatient::UsersUidList:  break;
        case IPatient::GroupsUidList: break;
        case IPatient::Id :           col = Constants::IDENTITY_ID;               break;
        case IPatient::Uid:           col = Constants::IDENTITY_UID;              break;
        case IPatient::FamilyUid:     col = Constants::IDENTITY_FAMILY_UID;       break;
        case IPatient::UsualName:
        {
            col = Constants::IDENTITY_USUALNAME;
            colsToEmit << Core::IPatient::FullName;
            break;
        }
        case IPatient::OtherNames:
        {
            col = Constants::IDENTITY_OTHERNAMES;
            colsToEmit << Core::IPatient::FullName;
            break;
        }
        case IPatient::Firstname:
        {
            col = Constants::IDENTITY_FIRSTNAME;
            colsToEmit << Core::IPatient::FullName;
            break;
        }
        case IPatient::GenderIndex:
        {
            col = Constants::IDENTITY_GENDER;
            QString g;
            switch (value.toInt()) {
            case 0: g = "M"; break;
            case 1: g = "F"; break;
            case 2: g = "H"; break;
            case 3: g = "K"; break;
            }

            // value not changed ? -> return
            if (d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER).data().toString() == g)
                return true;

            d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER), g, role);
            col=-1;
            colsToEmit << Core::IPatient::Gender << Core::IPatient::GenderPixmap << Core::IPatient::Photo_32x32 << Core::IPatient::Photo_64x64;
            break;
        }
        case IPatient::Gender:
        {
            const QString g = value.toString();
            QString toSave;
            switch (genders().indexOf(g)) {
            case 0 : toSave = "M"; break;
            case 1 : toSave = "F"; break;
            case 2:  toSave = "H"; break;
            case 3:  toSave = "K"; break;
            default: LOG_ERROR("Unknown gender " + g);
            }
            // value not changed ? -> return
            if (d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER).data().toString() == toSave)
                return true;

            d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), Constants::IDENTITY_GENDER), toSave, role);
            col = -1;
            colsToEmit << Core::IPatient::GenderIndex << Core::IPatient::GenderPixmap << Core::IPatient::Photo_32x32 << Core::IPatient::Photo_64x64;
            break;
        }
        case IPatient::DateOfBirth:
        {
            col = Constants::IDENTITY_DOB;
            colsToEmit << Core::IPatient::Age << Core::IPatient::YearsOld;
            break;
        }
        case IPatient::MaritalStatus: col = Constants::IDENTITY_MARITAL_STATUS;   break;
        case IPatient::DateOfDeath:   col = Constants::IDENTITY_DATEOFDEATH;      break;
        case IPatient::Profession:    col = Constants::IDENTITY_PROFESSION;       break;
        case IPatient::Street:
        {
            col = Constants::IDENTITY_ADDRESS_STREET;
            colsToEmit << Core::IPatient::FullAddress;
            break;
        }
        case IPatient::ZipCode:
        {
            col = Constants::IDENTITY_ADDRESS_ZIPCODE;
            colsToEmit << Core::IPatient::FullAddress;
            break;
        }
        case IPatient::City:
        {
            col = Constants::IDENTITY_ADDRESS_CITY;
            colsToEmit << Core::IPatient::FullAddress;
            break;
        }
        case IPatient::Country:
        {
            col = Constants::IDENTITY_ADDRESS_COUNTRY;
            colsToEmit << Core::IPatient::FullAddress;
            break;
        }
        case IPatient::StateProvince:
        {
            col = Constants::IDENTITY_ADDRESS_PROVINCE;
            colsToEmit << Core::IPatient::FullAddress;
            break;
        }
        case IPatient::AddressNote:
        {
            col = Constants::IDENTITY_ADDRESS_NOTE;
            colsToEmit << Core::IPatient::FullAddress;
            break;
        }
        case IPatient::Email:         col = Constants::IDENTITY_MAILS;            break;
        case IPatient::Tels:          col = Constants::IDENTITY_TELS;             break;
        case IPatient::MobilePhone:   col = Constants::IDENTITY_MOBILE_PHONE;     break;
        case IPatient::WorkPhone:     col = Constants::IDENTITY_WORK_PHONE;       break;
        case IPatient::Faxes:         col = Constants::IDENTITY_FAXES;            break;
        case IPatient::TitleIndex:    col = Constants::IDENTITY_TITLE;            break;
        case IPatient::Title:
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
                if (d->m_SqlPatient->data(d->m_SqlPatient->index(index.row(), col)).toInt() == id)
                    return true;

                d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), col), id, role);
                colsToEmit << Core::IPatient::TitleIndex << Core::IPatient::FullName;
            }
            col = -1;
            break;
        }
        case IPatient::Photo_32x32:
        case IPatient::Photo_64x64:
        {
            QPixmap pix = value.value<QPixmap>();
            QString patientUid = d->m_SqlPatient->index(index.row(), Constants::IDENTITY_UID).data().toString();
            d->savePatientPhoto(pix, patientUid);
            col = -1;
            colsToEmit << Core::IPatient::Photo_32x32 << Core::IPatient::Photo_64x64;
            break;
        }
        }

        if (col != -1) {
            // value not changed ? -> return
            if (d->m_SqlPatient->index(index.row(), col).data() == value)
                return true;
            if (value.isNull() && d->m_SqlPatient->index(index.row(), col).data().toString().isEmpty())
                return true;

            // value changed -> save to database
            const bool ok = d->m_SqlPatient->setData(d->m_SqlPatient->index(index.row(), col), value, role);
            if (!ok)
                LOG_QUERY_ERROR(d->m_SqlPatient->query());
        }

        // Emit data changed signals
        for(int i = 0; i < colsToEmit.count(); ++i) {
            QModelIndex idx = this->index(index.row(), colsToEmit.at(i), index.parent());
            Q_EMIT dataChanged(idx, idx);
        }
    }
    return true;
}

void PatientModel::setFilter(const QString &name, const QString &firstname, const QString &uuid, const FilterOn on)
{
    QString saveFilter = d->m_ExtraFilter;
    QHash<int, QString> where;
    // Calculate new filter
    switch (on) {
    case FilterOnFullName :
    {
        // WHERE (USUALNAME || OTHERNAMES || SURNAME LIKE '%') OR (USUALNAME LIKE '%')
        const QString &nameField = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_USUALNAME);
        const QString &secondField = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_OTHERNAMES);
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
        where.insert(Constants::IDENTITY_USUALNAME, QString("LIKE '%%1%'").arg(name));
        d->m_ExtraFilter = patientBase()->getWhereClause(Constants::Table_IDENT, where);
        break;
    }
    case FilterOnCity:
    {
        //            // WHERE CITY LIKE '%'
        //            d->m_ExtraFilter.clear();
        //            d->m_ExtraFilter = patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_ADDRESS_CITY) + " ";
        //            d->m_ExtraFilter += QString("LIKE '%%1%'").arg(filter);
        break;
    }
    case FilterOnUuid:
    {
        // WHERE PATIENT_UID='xxxx'
        d->m_ExtraFilter.clear();
        where.insert(Constants::IDENTITY_UID, QString("='%1'").arg(uuid));
        d->m_ExtraFilter = patientBase()->getWhereClause(Constants::Table_IDENT, where);
        break;
    }
    }
    if (saveFilter != d->m_ExtraFilter)
        d->refreshFilter();
}

QString PatientModel::filter() const
{
    if (d->m_SqlPatient)
        return d->m_SqlPatient->filter();
    return QString();
}

void PatientModel::emitPatientCreationOnSubmit(bool state)
{
    d->m_EmitCreationAtSubmit = state;
    if (!state) {
        // emit created uids
        for(int i = 0; i < d->m_CreatedPatientUid.count(); ++i) {
            Q_EMIT patientCreated(d->m_CreatedPatientUid.at(i));
        }
        d->m_CreatedPatientUid.clear();
    }
}

/**
 * Creates new patient in the model (but not submitted to the database).
 * Each UUID is defined by default.
 * All created patient are active and not virtual.
 */
bool PatientModel::insertRows(int row, int count, const QModelIndex &parent)
{
    //    qWarning() << "PatientModel::insertRows" << row << count << parent;
    bool ok = true;
    beginInsertRows(parent, row, row+count);
    for(int i=0; i < count; ++i) {
        if (!d->m_SqlPatient->insertRow(row+i, parent)) {
            ok = false;
            LOG_ERROR("Unable to create a new patient. PatientModel::insertRows()");
            continue;
        }
        // find an unused uuid
        bool findUuid = false;
        QString uuid;
        while (!findUuid) {
            // TODO: Take care to inifinite looping...
            uuid = Utils::Database::createUid();
            QString f = QString("%1='%2'").arg(patientBase()->fieldName(Constants::Table_IDENT, Constants::IDENTITY_UID), uuid);
            findUuid = (patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_UID, f) == 0);
        }
        if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_UID), uuid)) {
            ok = false;
            LOG_ERROR("Unable to setData to newly created patient.");
        }
        const QString rawUuid = user()->value(Core::IUser::Uuid).toString();
        const QString hyphenatedUuid = Utils::hyphenatedUuid(rawUuid);
        QByteArray binUuid = QUuid(hyphenatedUuid).toRfc4122();
        if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_USER_UUID), binUuid)) {
            ok = false;
            LOG_ERROR("Unable to setData to newly created patient.");
        }
        if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_ISACTIVE), 1)) {
            ok = false;
            LOG_ERROR("Unable to setData to newly created patient.");
        }
        if (!d->m_SqlPatient->setData(d->m_SqlPatient->index(row+i, Constants::IDENTITY_ISVIRTUAL), 0)) {
            ok = false;
            LOG_ERROR("Unable to setData to newly created patient.");
        }
        if (!d->m_EmitCreationAtSubmit) {
            Q_EMIT patientCreated(uuid);
        } else {
            d->m_CreatedPatientUid << uuid;
        }
    }
    endInsertRows();
    return ok;
}

bool PatientModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
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
    // Nothing to do ?
    //    if (!d->m_SqlPatient->isDirty())
    //        return true;

    // Submit the model to the database
    bool ok = d->m_SqlPatient->submitAll();
    if (!ok && d->m_SqlPatient->lastError().number() != -1) {
        LOG_ERROR(QString("Model not submitted. Error n° %1; %2")
                  .arg(d->m_SqlPatient->lastError().number())
                  .arg(d->m_SqlPatient->lastError().text()));
    }
    // emit created uids
    for(int i = 0; i < d->m_CreatedPatientUid.count(); ++i) {
        Q_EMIT patientCreated(d->m_CreatedPatientUid.at(i));
    }
    d->m_CreatedPatientUid.clear();
    return true;
}

/** Refresh the model. Reset the filter and reselect the database. */
bool PatientModel::refreshModel()
{
    // Keep the current patient persistent index in cache
    QModelIndex currentPatient = d->m_CurrentPatientIndex;
    // Reset the model
    beginResetModel();
    d->refreshFilter();
    endResetModel();
    // Redefine the current patient index
    d->m_CurrentPatientIndex = currentPatient;
    if (index(d->m_CurrentPatientIndex.row(), Core::IPatient::Uid).data().toString() != d->m_CurrentPatientUuid) {
        LOG_ERROR("After refreshing the patient model, the current patient uuid is wrong");
        return false;
    }
    return true;
}

/** Return the patient name corresponding to the \e uuids. Hash key represents the uuid while value is the full name. */
QHash<QString, QString> PatientModel::patientName(const QList<QString> &uuids)
{
    QHash<QString, QString> names;
    if (uuids.isEmpty())
        return names;
    QSqlDatabase DB = patientBase()->database();
    DB.transaction();
    QSqlQuery query(DB);
    const QStringList &titles = Trans::ConstantTranslations::titles();

    foreach(const QString &u, uuids) {
        if (u.isEmpty())
            continue;
        QHash<int, QString> where;
        where.insert(Constants::IDENTITY_UID, QString("='%1'").arg(u));
        QString req = patientBase()->select(Constants::Table_IDENT,
                                            QList<int>()
                                            << Constants::IDENTITY_TITLE
                                            << Constants::IDENTITY_USUALNAME
                                            << Constants::IDENTITY_OTHERNAMES
                                            << Constants::IDENTITY_FIRSTNAME,
                                            where);
        if (query.exec(req)) {
            if (query.next()) {
                QString title;
                int titleId = query.value(0).toInt();
                if (IN_RANGE_STRICT_MAX(titleId, 0, titles.count()))
                    title = titles.at(titleId);
                const QString &usualName = query.value(1).toString();
                const QString &otherNames = query.value(2).toString();
                const QString &firstName = query.value(3).toString();
                if (!otherNames.isEmpty()) {
                    names.insert(u, QString("%1 %2 - %3 %4")
                                 .arg(title, usualName, otherNames, firstName).simplified());
                } else {
                    names.insert(u, QString("%1 %2 %3")
                                 .arg(title, usualName, firstName).simplified());
                }
            }
        } else {
            LOG_QUERY_ERROR_FOR("PatientModel", query);
            query.finish();
            DB.rollback();
            return names;
        }
        query.finish();
    }
    DB.commit();
    return names;
}

/** Return the Uuid of the patient according to the \e index */
QString PatientModel::patientUuid(const QModelIndex &index) const
{
    QModelIndex idx = this->index(index.row(), Core::IPatient::Uid);
    return data(idx).toString();
}

