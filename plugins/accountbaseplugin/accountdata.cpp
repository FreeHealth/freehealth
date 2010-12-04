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
    QHash<int, QVariant> m_DbDatas;
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
    return d->m_DbDatas.value(ref);
}

bool AccountData::setValue(const int ref, const QVariant &value)
{
    if (ref >= Max_Params || ref < 0)
        return false;

    d->m_DbDatas.insert(ref, value);

    if (!d->m_Dirty.contains(ref))
        d->m_Dirty.append(ref);

    return true;

}

AmountModel *AccountData::amountModel() const
{
    return 0;
}

/** \brief This member should only be used by AcocuntBase. \e field is a representation of AccountDb::Constants::TableAcc_Fields. */
void AccountData::setDatasFromDb(const int ref, const QVariant &value)
{
    switch (ref) {
    case Constants::ACCOUNT_ID : d->m_DbDatas.insert(Id, value); break;
    case Constants::ACCOUNT_UID : d->m_DbDatas.insert(Uid, value); break;
    case Constants::ACCOUNT_USER_UID : d->m_DbDatas.insert(UserUid, value); break;
    case Constants::ACCOUNT_PATIENT_UID : d->m_DbDatas.insert(PatientUid, value); break;
    case Constants::ACCOUNT_PATIENT_NAME : d->m_DbDatas.insert(PatientFullName, value); break;
    case Constants::ACCOUNT_SITE_ID: d->m_DbDatas.insert(SiteId, value); break;
    case Constants::ACCOUNT_INSURANCE_ID: d->m_DbDatas.insert(InsuranceId, value); break;
    case Constants::ACCOUNT_DATE : d->m_DbDatas.insert(Date, value); break;
//    case MedicalProcedureHtml : return "/** \todo here */";
    case Constants::ACCOUNT_COMMENT : d->m_DbDatas.insert(Comment, value); break;
    case Constants::ACCOUNT_CASHAMOUNT: d->m_DbDatas.insert(Cash, value); break;
    case Constants::ACCOUNT_CHEQUEAMOUNT: d->m_DbDatas.insert(Cheque, value); break;
    case Constants::ACCOUNT_VISAAMOUNT: d->m_DbDatas.insert(Visa, value); break;
    case Constants::ACCOUNT_INSURANCEAMOUNT: d->m_DbDatas.insert(Insurance, value); break;
    case Constants::ACCOUNT_OTHERAMOUNT: d->m_DbDatas.insert(Other, value); break;
    case Constants::ACCOUNT_DUEAMOUNT: d->m_DbDatas.insert(DueAmount, value); break;
    case Constants::ACCOUNT_DUEBY : d->m_DbDatas.insert(DueBy, value); break;
    case Constants::ACCOUNT_ISVALID :d->m_DbDatas.insert(IsValid, value); break;
    case Constants::ACCOUNT_TRACE : d->m_DbDatas.insert(Trace, value); break;
    default : break;
    }
}
