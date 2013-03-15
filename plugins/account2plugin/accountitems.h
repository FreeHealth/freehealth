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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ACCOUNT2_ACCOUNTITEMS
#define ACCOUNT2_ACCOUNTITEMS

#include <account2plugin/account2_exporter.h>
#include <QString>
#include <QVariant>
#include <QDateTime>

/**
 * \file accountitems.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 13 Mar 2013
*/

namespace Account2 {
namespace Internal {
class AccountBase;
class AccountBasePrivate;
}

class ACCOUNT2_EXPORT BasicsOfItem
{
    friend class Account2::Internal::AccountBase;
    friend class Account2::Internal::AccountBasePrivate;
public:
    BasicsOfItem() : _id(-1), _sign_id(-1), _valid(true), _modified(false) {}
    virtual ~BasicsOfItem() {}

    virtual int id() const {return _id;}
    virtual void setId(int id) {_modified=true; _id = id;}

    virtual QString uuid() const {return _uid;}
    virtual void setUuid(const QString &uid) {_modified=true; _uid=uid;}

    virtual bool isValid() const {return _valid;}
    virtual void setValid(bool state) {_modified=true; _valid=state;}

    virtual int signatureId() const {return _sign_id;}
    virtual void setSignatureId(int id) {_modified=true; _sign_id = id;}

    virtual void setModified(bool state) {_modified = state;}
    virtual bool isModified() const {return _modified;}

protected:
    int _id, _sign_id;
    bool _valid, _modified;
    QString _uid;
};

class ACCOUNT2_EXPORT DatesOfItem : public BasicsOfItem
{
    friend class Account2::Internal::AccountBase;
    friend class Account2::Internal::AccountBasePrivate;
public:
    enum DateType {
        // Date_Quotation, --> eq Date_Creation in Quotation object
        Date_MedicalRealisation = 0,
        Date_Invocing,
        Date_Payment,
        Date_Banking,
        Date_Accountancy,
        Date_Creation,
        Date_Update,
        Date_Validation,
        Date_Annulation,
        Date_ValidityPeriodStart,
        Date_ValidityPeriodEnd,
        Date_MaxParam
    };

    DatesOfItem() : _date_did(-1) {}
    virtual ~DatesOfItem() {}

    virtual QDateTime date(DateType type) const;
    virtual void setDate(int type, const QDateTime &datetime);
    virtual void setDate(int type, const QDate &date);

protected: // for database management
    int dateDid() const {return _date_did;}
    void setDateDid(int id) {setModified(true); _date_did = id;}
    static QString dateTypeUid(DateType type);

private:
    int _date_did;
    QHash<int, QDateTime> _dates;
};

class ACCOUNT2_EXPORT Fee : public DatesOfItem
{
    friend class Account2::Internal::AccountBase;
    friend class Account2::Internal::AccountBasePrivate;
public:
    Fee() : _mp_id(-1), _amount(0.) {}
    virtual ~Fee() {}

    virtual void setAmount(double amount) {_modified = true; _amount=amount;}
    virtual double amount() const {return _amount;}
    bool isNull() {return _amount <= 0.0;}

    virtual QString userUid() const {return _userUid;}
    virtual void setUserUuid(const QString &uid) {_modified=true; _userUid=uid;}

    virtual QString patientUid() const {return _patientUid;}
    virtual void setPatientUuid(const QString &uid) {_modified=true; _patientUid=uid;}

    virtual QString type() const {return _type;}
    virtual void setType(const QString &type) {_modified=true; _type=type;}

    virtual QString comment() const {return _comment;}
    virtual void setComment(const QString &comment) {_modified=true; _comment=comment;}

protected: // For database management
    int mpId() const {return _mp_id;}
    void setMpId(int id) {_modified=true; _mp_id = id;}

private:
    int _mp_id;
    double _amount;
    QString _userUid, _patientUid, _type, _comment;
};

class ACCOUNT2_EXPORT Payment : public DatesOfItem
{
    friend class Account2::Internal::AccountBase;
    friend class Account2::Internal::AccountBasePrivate;
public:
    enum PaymentType {
        Cash = 0,
        Cheque,
        VISA,
        BankTransfer,
        InsuranceDelayed,
        Other
    };

    Payment() : _quotationId(-1) {}
    virtual ~Payment() {}

    virtual void setAmount(double amount) {_modified = true; _amount=amount;}
    virtual double amount() const {return _amount;}
    bool isNull() const {return _amount <= 0.0;}

    virtual PaymentType type() const {return _type;}
    virtual void setType(PaymentType type) {_modified=true; _type=type;}

    virtual QString comment() const {return _comment;}
    virtual void setComment(const QString &comment) {_modified=true; _comment=comment;}

    virtual void addFee(const Fee &fee) {_fees << fee;}
    virtual QList<Fee> fees() const {return _fees;}

//    static QString typeToString(ScriptType type);
//    static QString typeToXml(ScriptType type);
//    static ScriptType typeFromXml(const QString &xml);
//    virtual QString toXml() const;
//    static AlertScript fromDomElement(const QDomElement &element);

//    bool operator<(const AlertScript &script) const;

protected: // For database management
    QList<int> feesId() const {return _feesId;}
    void addFeesId(int id) {_modified=true; _feesId << id;}

    int quotationId() const {return _quotationId;}
    void setQuotationId(int id) {_modified=true; _quotationId = id;}

private:
    int _quotationId;
    double _amount;
    QList<int> _feesId;
    QList<Fee> _fees;
    PaymentType _type;
    QString _comment;
};

class ACCOUNT2_EXPORT Quotation : public DatesOfItem
{
    friend class Account2::Internal::AccountBase;
    friend class Account2::Internal::AccountBasePrivate;
public:
    Quotation() : _total(0.0) {}
    virtual ~Quotation() {}

    virtual QString creatorUid() const {return _uid;}
    virtual void setCreatorUuid(const QString &uid) {_modified=true; _uid=uid;}

    virtual QString label() const {return _label;}
    virtual void setLabel(const QString &label) {_modified=true; _label=label;}

    virtual QString userComment() const {return _userComment;}
    virtual void setUserComment(const QString &comment) {_modified=true; _userComment=comment;}

    virtual void setTotalAmount(double total) {_modified=true; _total=total;}
    virtual double totalAmount() const {return _total;}

    virtual void addFee(const Fee &fee) {_fees << fee;}
    virtual QList<Fee> fees() const {return _fees;}

protected:  // for database management
    QList<int> feesId() const {return _feesId;}
    void addFeesId(int id) {_modified=true; _feesId << id;}

private:
    QList<int> _feesId;
    QList<Fee> _fees;
    double _total;
    QString _label, _uid, _userComment;
};

class ACCOUNT2_EXPORT Banking : public DatesOfItem
{
    friend class Account2::Internal::AccountBase;
    friend class Account2::Internal::AccountBasePrivate;
public:
    Banking() : _bkid(-1), _total(0.0) {}
    virtual ~Banking() {}

    virtual void setBankAccount(const QString &uid) {_modified=true; _bankAccount=uid;}
    virtual QString bankAccount() const {return _bankAccount;}

    virtual void setTotalAmount(double total) {_modified=true; _total=total;}
    virtual double totalAmount() const {return _total;}

    virtual void addPayment(const Payment &payment) {_payments << payment;}
    virtual QList<Payment> payments() const {return _payments;}

protected:  // for database management
    QList<int> paymentsId() const {return _paymentsId;}
    void addPaymentsId(int id) {_modified=true; _paymentsId << id;}

    int bankingBkid() const {return _bkid;}
    void setBankingBkid(int id) {_modified=true; _bkid = id;}

private:
    int _bkid;
    double _total;
    QString _bankAccount;
    QList<int> _paymentsId;
    QList<Payment> _payments;
};

}  // namespace Account2

//ACCOUNT2_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertItem &c);
//ACCOUNT2_EXPORT QDebug operator<<(QDebug dbg, const Alert::AlertItem *c);

#endif  // ACCOUNT2_ACCOUNTITEMS

