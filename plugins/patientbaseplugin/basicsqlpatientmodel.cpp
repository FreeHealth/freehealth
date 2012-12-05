/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Patients::Internal::BasicSqlPatientModel
 * \brief short description of class
 *
 * Long description of class
 * \sa Patients::
 */

#include "basicsqlpatientmodel.h"
#include "patientcore.h"
#include "patientbase.h"
#include "constants_db.h"
#include "constants_settings.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <medicalutils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_titles.h>

#include <QSqlTableModel>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Patients::Internal::PatientBase *patientBase() {return Patients::PatientCore::instance()->patientBase();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

namespace Patients {
namespace Internal {
class BasicSqlPatientModelPrivate
{
public:
    BasicSqlPatientModelPrivate(BasicSqlPatientModel *parent) :
        _sqlPatient(0),
        q(parent)
    {
    }
    
    ~BasicSqlPatientModelPrivate()
    {
    }

    // TODO: put this in Core::ITheme
    QIcon iconizedGender(const QModelIndex &index)
    {
        const QString &g = _sqlPatient->data(_sqlPatient->index(index.row(), Constants::IDENTITY_GENDER)).toString();
        if (g=="M") {
            return theme()->icon(Core::Constants::ICONMALE);
        } else if (g=="F") {
            return theme()->icon(Core::Constants::ICONFEMALE);
        } else if (g=="H") {
            return theme()->icon(Core::Constants::ICONHERMAPHRODISM);
        }
        return QIcon();
    }

    void refreshFilter()
    {
    }

public:
    QSqlTableModel *_sqlPatient;
    QString _filter;
    
private:
    BasicSqlPatientModel *q;
};
}  // namespace Internal
} // end namespace Patients

/*! Constructor of the Patients::Internal::BasicSqlPatientModel class */
BasicSqlPatientModel::BasicSqlPatientModel(QObject *parent) :
    QAbstractListModel(parent),
    d(new BasicSqlPatientModelPrivate(this))
{
    onCoreDatabaseServerChanged();
    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

/*! Destructor of the Patients::Internal::BasicSqlPatientModel class */
BasicSqlPatientModel::~BasicSqlPatientModel()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool BasicSqlPatientModel::initialize()
{
}

void BasicSqlPatientModel::onCoreDatabaseServerChanged()
{
    // Destroy old model and recreate it
    if (d->_sqlPatient) {
        disconnect(d->_sqlPatient);
        delete d->_sqlPatient;
    }
    d->_sqlPatient = new QSqlTableModel(this, patientBase()->database());
    d->_sqlPatient->setTable(patientBase()->table(Constants::Table_IDENT));
    Utils::linkSignalsFromFirstModelToSecondModel(d->_sqlPatient, this, false);

    d->refreshFilter();
}

int BasicSqlPatientModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid())
        return 0;
    return d->_sqlPatient->rowCount();
}

int BasicSqlPatientModel::columnCount(const QModelIndex &) const
{
    return Core::IPatient::NumberOfColumns;
}

int BasicSqlPatientModel::numberOfFilteredPatients() const
{
    return patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_BIRTHNAME, d->_sqlPatient->filter());
}

void BasicSqlPatientModel::fetchMore(const QModelIndex &parent)
{
    if (d->_sqlPatient)
        d->_sqlPatient->fetchMore(parent);
}

bool BasicSqlPatientModel::canFetchMore(const QModelIndex &parent) const
{
    if (d->_sqlPatient)
        return d->_sqlPatient->canFetchMore(parent);
    return false;
}

QVariant BasicSqlPatientModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DecorationRole) {
        using namespace Core;
        if (index.column() == IPatient::BirthName) {
            return d->iconizedGender(index);
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
        case IPatient::BirthName:     col = Constants::IDENTITY_BIRTHNAME;       break;
        case IPatient::SecondName:    col = Constants::IDENTITY_SECONDNAME;        break;
        case IPatient::Firstname:     col = Constants::IDENTITY_FIRSTNAME;           break;
        case IPatient::Gender:        col = Constants::IDENTITY_GENDER;            break;
        case IPatient::GenderIndex:
            {
            // TODO: put this in a separate method/class, there is much duplication of gender (de)referencing in FMF
                const QString &g = d->_sqlPatient->index(index.row(), Constants::IDENTITY_GENDER).data().toString();
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
            QModelIndex idx = d->_sqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            QDate dob = d->_sqlPatient->data(idx).toDate();
            if (role==Qt::DisplayRole) {
                return dob;
            } else if (role==Qt::ToolTipRole) {
                return QString("%1; %2").arg(QLocale().toString(dob, QLocale().dateFormat(QLocale::LongFormat))).arg(MedicalUtils::readableAge(dob));
            }
            return dob;
            break;
        }
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
                int titleIndex = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), col)).toInt();
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
                const QString &name = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_BIRTHNAME)).toString();
                const QString &sec = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_SECONDNAME)).toString();
                const QString &first = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_FIRSTNAME)).toString();
                QString title;
                // add title
                int titleIndex = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_TITLE)).toInt();
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

                if (!sec.isEmpty()) {
                    return QString("%1 - %2 %3").arg(title+name, sec, first);
                } else {
                    return QString("%1 %2").arg(title+name, first);
                }
                break;
            }
        case IPatient::FullAddress:
            {
                const QString &street = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_STREET)).toString();
                const QString &city = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_ADRESS_CITY)).toString();
                const QString &zip = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_ZIPCODE)).toString();
                QString country = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_ADDRESS_COUNTRY)).toString();
                country = Utils::countryIsoToName(country);
                return QString("%1 %2 %3 %4").arg(street, city, zip, country).simplified();
            }
        case IPatient::Age:
        {
            QModelIndex idx = d->_sqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            return MedicalUtils::readableAge(d->_sqlPatient->data(idx).toDate());
        }
        case IPatient::YearsOld:
        {
            QModelIndex idx = d->_sqlPatient->index(index.row(), Constants::IDENTITY_DOB);
            return MedicalUtils::ageYears(d->_sqlPatient->data(idx).toDate());
        }
        case IPatient::IconizedGender: return d->iconizedGender(index);
        }
        QVariant r = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), col), role);
        switch (index.column()) {
        case Core::IPatient::DateOfBirth:
        case Core::IPatient::DateOfDeath:
            return QLocale().toString(r.toDate(), tkTr(Trans::Constants::DATEFORMAT_FOR_MODEL));
        default:
            return r;
        }
    }
    else if (role==Qt::DecorationRole) {
        switch (index.column()) {
        case Core::IPatient::IconizedGender: return d->iconizedGender(index);
        }
    } else if (role==Qt::BackgroundRole) {
        if (settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool()) {
            const QString &g = d->_sqlPatient->data(d->_sqlPatient->index(index.row(), Constants::IDENTITY_GENDER)).toString();
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

bool BasicSqlPatientModel::setFilter(const BasicSqlPatientFilter &filter)
{
}
