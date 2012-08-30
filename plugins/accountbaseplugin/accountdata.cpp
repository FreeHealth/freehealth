/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "accountdata.h"
#include "constants.h"
#include "amountmodel.h"

#include <QVector>


using namespace AccountDB;

namespace AccountDB {
namespace Internal {

class AccountDataPrivate
{
public:
    AccountDataPrivate(AccountData *parent) : q(parent)
    {}

public:
    QHash<int, QVariant> m_DbData;
    QVector<int> m_Dirty;

private:
    AccountData *q;
};

}  // End namespace Internal
}  // End namespace AccountDB


AccountData::AccountData() :
        d(0)
{
    d = new Internal::AccountDataPrivate(this);
}

AccountData::~AccountData()
{
    if (d)
        delete d;
    d = 0;
}

QVariant AccountData::value(const int ref) const
{
    if (ref >= Max_Params || ref < 0)
        return QVariant();
    return d->m_DbData.value(ref);
}

bool AccountData::setValue(const int ref, const QVariant &value)
{
    if (ref >= Max_Params || ref < 0)
        return false;

    d->m_DbData.insert(ref, value);

    if (!d->m_Dirty.contains(ref))
        d->m_Dirty.append(ref);

    return true;

}

AmountModel *AccountData::amountModel() const
{
    return 0;
}

/** \brief This member should only be used by AcocuntBase. \e field is a representation of AccountDb::Constants::TableAcc_Fields. */
void AccountData::setDataFromDb(const int ref, const QVariant &value)
{
    switch (ref) {
    case Constants::ACCOUNT_ID : d->m_DbData.insert(Id, value); break;
    case Constants::ACCOUNT_UID : d->m_DbData.insert(Uid, value); break;
    case Constants::ACCOUNT_USER_UID : d->m_DbData.insert(UserUid, value); break;
    case Constants::ACCOUNT_PATIENT_UID : d->m_DbData.insert(PatientUid, value); break;
    case Constants::ACCOUNT_PATIENT_NAME : d->m_DbData.insert(PatientFullName, value); break;
    case Constants::ACCOUNT_SITE_ID: d->m_DbData.insert(SiteId, value); break;
    case Constants::ACCOUNT_INSURANCE_ID: d->m_DbData.insert(InsuranceId, value); break;
    case Constants::ACCOUNT_DATE : d->m_DbData.insert(Date, value); break;
//    case MedicalProcedureHtml : return "// TODO: here */"; //TODO: remove this line?
    case Constants::ACCOUNT_COMMENT : d->m_DbData.insert(Comment, value); break;
    case Constants::ACCOUNT_CASHAMOUNT: d->m_DbData.insert(Cash, value); break;
    case Constants::ACCOUNT_CHEQUEAMOUNT: d->m_DbData.insert(Cheque, value); break;
    case Constants::ACCOUNT_VISAAMOUNT: d->m_DbData.insert(Visa, value); break;
    case Constants::ACCOUNT_INSURANCEAMOUNT: d->m_DbData.insert(Insurance, value); break;
    case Constants::ACCOUNT_OTHERAMOUNT: d->m_DbData.insert(Other, value); break;
    case Constants::ACCOUNT_DUEAMOUNT: d->m_DbData.insert(DueAmount, value); break;
    case Constants::ACCOUNT_DUEBY : d->m_DbData.insert(DueBy, value); break;
    case Constants::ACCOUNT_ISVALID :d->m_DbData.insert(IsValid, value); break;
    case Constants::ACCOUNT_TRACE : d->m_DbData.insert(Trace, value); break;
    default : break;
    }
}
