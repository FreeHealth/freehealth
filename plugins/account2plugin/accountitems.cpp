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
#include "accountitems.h"

#include <utils/log.h>

#include <QStringList>
#include <QDebug>

namespace Account2 {

QDateTime VariableDatesItem::date(DateType type) const
{
    return _dates.value(type, QDateTime());
}

void VariableDatesItem::setDate(int type, const QDateTime &datetime)
{
    _dates.insert(type, datetime);
}

void VariableDatesItem::setDate(int type, const QDate &date)
{
    _dates.insert(type, QDateTime(date, QTime(0,0,0)));
}

/** Return a persistent in time uuid for each date type for the database management */
QString VariableDatesItem::dateTypeToSql(DateType type)
{
    switch (type) {
    case Date_MedicalRealisation: return "med_real";
    case Date_Invocing: return "inv";
    case Date_Payment: return "pay";
    case Date_Banking: return "bkg";
    case Date_Accountancy: return "acc";
    case Date_Creation: return "crea";
    case Date_Update: return "upd";
    case Date_Validation: return "val";
    case Date_Annulation: return "ann";
    case Date_ValidityPeriodStart: return "validitystart";
    case Date_ValidityPeriodEnd: return "validityend";
    default: return QString::null;
    }
    return QString::null;
}

/** Return the date type from the persistent in time uuid (for the database management) */
VariableDatesItem::DateType VariableDatesItem::dateTypeFromSql(const QString &type)
{
    if (type=="med_real")
        return Date_MedicalRealisation;
    if (type=="inv")
        return Date_Invocing;
    if (type=="pay")
        return Date_Payment;
    if (type=="bkg")
        return Date_Banking;
    if (type=="acc")
        return Date_Accountancy;
    if (type=="crea")
        return Date_Creation;
    if (type=="upd")
        return Date_Update;
    if (type=="val")
        return Date_Validation;
    if (type=="ann")
        return Date_Annulation;
    if (type=="validitystart")
        return Date_ValidityPeriodStart;
    if (type=="validityend")
        return Date_ValidityPeriodEnd;
    Q_ASSERT(false);
    LOG_ERROR_FOR("VariableDatesItem", "Unknown type: " + type);
    return Date_Creation;
}

/** Add a Fee to a Payment. WARNING: the fee must be already saved into the database and/or have its id() defined. */
void Payment::addFee(const Fee &fee)
{
    Q_ASSERT(fee.id() >= 0);
    _fees << fee;
    _feesId << fee.id();
}

/** Return a persistent in time uuid for each type of payment for the database management */
QString Payment::typeToSql(PaymentType type)
{
    switch (type) {
    case Cash: return "cash";
    case Cheque: return "chq";
    case VISA: return "visa";
    case BankTransfer: return "bktfrt";
    case InsuranceDelayed: return "delay";
    case Other: return "other";
    }
}

/** Return the payment type from the persistent in time uuid (for the database management) */
Payment::PaymentType Payment::typeFromSql(const QString &type)
{
    if (type=="cash")
        return Cash;
    if (type=="chq")
        return Cheque;
    if (type=="visa")
        return VISA;
    if (type=="bktfrt")
        return BankTransfer;
    if (type=="delay")
        return InsuranceDelayed;
    if (type=="other")
        return Other;
    Q_ASSERT(false);
    LOG_ERROR_FOR("Payment", "Unknown type: " + type);
    return Other;
}

bool Banking::canComputeTotalAmount()
{
    return _payments.count() == _paymentsId.count();
}

bool Banking::computeTotalAmount()
{
    if (!canComputeTotalAmount())
        return false;
    _total = 0.0;
    foreach(const Payment &pay, _payments)
        _total += pay.amount();
    return true;
}

void Banking::addPayment(const Payment &payment)
{
    _payments << payment;
    _paymentsId << payment.id();
}

} // namespace Account2


QDebug operator<<(QDebug dbg, const Account2::Fee &c)
{
    QStringList s;
    s << "Account2::Fee(" + QString::number(c.id());
    if (c.isValid()) {
        if (c.isModified())
            s << "valid*";
        else
            s << "valid";
    } else {
        if (c.isModified())
            s << "notValid*";
        else
            s << "notValid";
    }
    s << "amount: " + QString::number(c.amount(), 'f', 6);
    s << "user: " + c.userUid();
    s << "patient: " + c.patientUid();
    s << "type: " + c.type();
    s << "comment: " + c.comment();
    for(int i = 0; i < Account2::VariableDatesItem::Date_MaxParam; ++i) {
        if (c.date(Account2::Fee::DateType(i)).isValid())
            s << QString("date: %1 - %2").arg(i).arg(c.date(Account2::Fee::DateType(i)).toString(Qt::ISODate));
    }
    dbg.nospace() << s.join(",\n           ") + ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Account2::Payment &c)
{
    QStringList s;
    s << "Account2::Payment(" + QString::number(c.id());
    if (c.isValid()) {
        if (c.isModified())
            s << "valid*";
        else
            s << "valid";
    } else {
        if (c.isModified())
            s << "notValid*";
        else
            s << "notValid";
    }
    s << "amount: " + QString::number(c.amount(), 'f', 6);
    s << "type: " + c.type();
    s << "comment: " + c.comment();
    foreach(const Account2::Fee &fee, c.fees()) {
        s << "Fee: " + QString::number(fee.id()) + "; amount: " + QString::number(fee.amount(), 'f', 6);;
    }

    for(int i = 0; i < Account2::VariableDatesItem::Date_MaxParam; ++i) {
        if (c.date(Account2::Fee::DateType(i)).isValid())
            s << QString("date: %1 - %2").arg(i).arg(c.date(Account2::Fee::DateType(i)).toString(Qt::ISODate));
    }
    dbg.nospace() << s.join(",\n           ") + ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Account2::Banking &c)
{
    QStringList s;
    s << "Account2::Banking(" + QString::number(c.id());
    if (c.isValid()) {
        if (c.isModified())
            s << "valid*";
        else
            s << "valid";
    } else {
        if (c.isModified())
            s << "notValid*";
        else
            s << "notValid";
    }
    s << "bkAccUid: " + c.bankAccountUuid();
    // s << "bkAccId: " + c.bankAccountId();
    s << "total: " + QString::number(c.totalAmount(), 'f', 6);
    foreach(const Account2::Payment &pay, c.payments()) {
        s << "Payment: " + QString::number(pay.id()) + "; amount: " + QString::number(pay.amount(), 'f', 6);
    }

    for(int i = 0; i < Account2::VariableDatesItem::Date_MaxParam; ++i) {
        if (c.date(Account2::Fee::DateType(i)).isValid())
            s << QString("date: %1 - %2").arg(i).arg(c.date(Account2::Fee::DateType(i)).toString(Qt::ISODate));
    }
    dbg.nospace() << s.join(",\n           ") + ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Account2::Quotation &c)
{
    return dbg.space();
}
