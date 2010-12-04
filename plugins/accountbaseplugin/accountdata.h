#ifndef ACCOUNTDATA_H
#define ACCOUNTDATA_H

#include <accountbaseplugin/accountbase_exporter.h>
#include <QVariant>

namespace AccountDB {
class AmountModel;
class AccountBase;

namespace Internal {
class AccountDataPrivate;
}


class ACCOUNTBASE_EXPORT AccountData
{
    friend class AccountDB::AccountBase;

public:
    enum DataRepresentationforValue {
        Id = 0,
        Uid,
        UserUid,
        PatientUid,
        PatientFullName,
        SiteId,
        InsuranceId,
        Date,  // there must be more types of dates
        MedicalProcedureHtml,
        Comment,

        // Amounts
        Cash,
        Cheque,
        Visa,
        Insurance,
        Other,
        DueAmount,
        DueBy,

        // Validity and time memory
        IsValid,
        Trace,
        Max_Params
    };

    AccountData();
    ~AccountData();

    QVariant value(const int ref) const;
    bool setValue(const int ref, const QVariant &value);

     AmountModel *amountModel() const;

protected:
    void setDatasFromDb(const int field, const QVariant &value);

private:
    Internal::AccountDataPrivate *d;
};

}  // End AccountDB

#endif // ACCOUNTDATA_H
