/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *                                                                         *
 *  (C) 2013-now by Christian A Reiter and Eric Maeker                     *
 *  (C) 2010-2012 by Pierre-Marie Desombre and Eric Maeker                 *
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
 *  Main Developers :  Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT2_ACCOUNTBASERESULT_H
#define ACCOUNT2_ACCOUNTBASERESULT_H

#include <account2plugin/accountitems.h>

/**
 * \file accountbaseresult.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 14 Mar 2013
*/

namespace Account2 {
namespace Internal {

class AccountBaseResult
{
public:
    AccountBaseResult() : _success(true) {}
    ~AccountBaseResult() {}

    void setSuccessfull(bool success) {_success=success;}
    bool isSuccessfull() const {return _success;}

    void setErrorMessage(const QString &msg) {_errorMsg=msg;}
    QString errorMessage() const {return _errorMsg;}

    void setFees(const QList<Fee> &list) {_fees = list;}
    void setPayments(const QList<Payment> &list) {_payments = list;}
    void setBankings(const QList<Banking> &list) {_bankings = list;}
    void setQuotations(const QList<Quotation> &list) {_quotations = list;}

    void addFee(const Fee &fee) {_fees << fee;}
    void addPayment(const Payment &pay) {_payments << pay;}
    void addBanking(const Banking &bkg) {_bankings << bkg;}
    void addQuotation(const Quotation &quot) {_quotations << quot;}

    const QList<Fee> &fees() const {return _fees;}
    const QList<Payment> &payments() const {return _payments;}
    const QList<Banking> &bankings() const {return _bankings;}
    const QList<Quotation> &quotations() const {return _quotations;}

private:
    bool _success;
    QString _errorMsg;
    QList<Fee> _fees;
    QList<Payment> _payments;
    QList<Banking> _bankings;
    QList<Quotation> _quotations;
};

} // namespace Internal
} // namespace Account2

#endif // ACCOUNT2_ACCOUNTBASERESULT_H
