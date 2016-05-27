/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
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

/*!
 * \brief The AccountData class holds the data that is used by the \sa AccountDB::AmountModel.
 *
 *
 */
class ACCOUNTBASE_EXPORT AccountData
{
    friend class AccountDB::AccountBase;

public:
    /*! \enum AccountDb::DataRepresentationforValue
     * Data representation values that are used in various functions within AccountData.
     */
    enum DataRepresentationforValue {
        Id = 0,
        Uid,                    /*!< Unique ID of the amount value */
        UserUid,                /*!< Unique ID of the current user that saved this amount*/
        PatientUid,             /*!< Unique ID of the patient, defined in \sa IPatient::uuid() */
        PatientFullName,        /*!< Full name of the patient */
        SiteId,
        InsuranceId,            /*!< ID of the insurance (if payment is \e insurance */
        Date,                   // TODO: there must be more types of dates
        MedicalProcedureHtml,
        Comment,                /*!< A general comment */

        // Amounts
        Cash,                   /*!< Cash values */
        Cheque,                 /*!< Cheque */
        Visa,                   /*!< Credit Card */  //TODO: use a more generic "credit card" approach
        Insurance,              /*!< Insurance settlement */
        Other,                  /*!< Other payment, not listed */
        DueAmount,              /*!< Amount of due */
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
    void setDataFromDb(const int ref, const QVariant &value);

private:
    Internal::AccountDataPrivate *d;
};

}  // End AccountDB

#endif // ACCOUNTDATA_H
