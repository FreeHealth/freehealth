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
/**
 * \class Account2::AccountBase
 * Manage all database access. Get the single instance of the object from
 * Account2::AccountCore
 */
#include "accountbase.h"
#include "constants_db.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/icommandline.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>
#include <QMultiHash>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QSet>

using namespace Account2;
using namespace Internal;
using namespace Trans::ConstantTranslations;

enum { WarnDebugMessage = false };

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

static inline bool connectDatabase(QSqlDatabase DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("AccountBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()), __FILE__, line);
            return false;
        }
    }
    return true;
}

namespace Account2 {
namespace Internal {
class AccountBasePrivate
{
public:
    AccountBasePrivate(AccountBase *base) :
        m_LogChrono(false), m_initialized(false),
        _transaction(false),
        q(base)
    {
        r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    }

    ~AccountBasePrivate()
    {
    }

    Fee getFee(const QString &uuid)
    {
        Fee fee;
        if (!connectDatabase(q->database(), __LINE__))
            return fee;
        bool transactionCreated = false;
        if (!_transaction) {
            q->database().transaction();
            transactionCreated = true;
            _transaction = true;
        }

        QHash<int, QString> where;
        // Get Payment table
        where.insert(Constants::FEES_ID, QString("='%1'").arg(uuid));
        QSqlQuery query(q->database());
        if (query.exec(q->select(Constants::Table_Fees, where))) {
            if (query.next()) {
                fee.setId(query.value(Constants::FEES_ID).toInt());
                fee.setValid(query.value(Constants::FEES_ISVALID).toBool());
                fee.setUserUuid(query.value(Constants::FEES_USER_UID).toString());
                fee.setPatientUuid(query.value(Constants::FEES_PATIENT_UID).toString());
                fee.setLabel(query.value(Constants::FEES_LABEL).toString());
                fee.setAmount(query.value(Constants::FEES_AMOUNT).toDouble());
                fee.setType(query.value(Constants::FEES_TYPE).toString());
                fee.setDateDid(query.value(Constants::FEES_DATE_DID).toInt());
                fee.setComment(query.value(Constants::FEES_COMMENT).toString());
                fee.setMpId(query.value(Constants::FEES_MP_ID).toInt());
                fee.setSignatureId(query.value(Constants::FEES_SIGN_ID).toInt());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return fee;
        }
        query.finish();

        getDates(fee);

        if (transactionCreated) {
            q->database().commit();
            _transaction = false;
        }
        return fee;
    }

    Payment getPayment(const QString &uuid)
    {
        Payment pay;
        if (!connectDatabase(q->database(), __LINE__))
            return pay;
        bool transactionCreated = false;
        if (!_transaction) {
            q->database().transaction();
            transactionCreated = true;
            _transaction = true;
        }

        QHash<int, QString> where;
        // Get Payment table
        where.insert(Constants::PAYMENT_ID, QString("='%1'").arg(uuid));
        QSqlQuery query(q->database());
        if (query.exec(q->select(Constants::Table_Payments, where))) {
            if (query.next()) {
                pay.setId(query.value(Constants::PAYMENT_ID).toInt());
                pay.setQuotationId(query.value(Constants::PAYMENT_QUOT_ID).toInt());
                pay.setValid(query.value(Constants::PAYMENT_ISVALID).toBool());
                pay.setAmount(query.value(Constants::PAYMENT_AMOUNT).toDouble());
                pay.setType(Payment::typeFromSql(query.value(Constants::PAYMENT_TYPE).toString()));
                pay.setDateDid(query.value(Constants::PAYMENT_DATE_DID).toInt());
                pay.setComment(query.value(Constants::PAYMENT_COMMENT).toString());
                pay.setSignatureId(query.value(Constants::PAYMENT_SIGN_ID).toInt());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return pay;
        }
        query.finish();

        // Get Fees from PaymentsFeesLink table
        where.clear();
        where.insert(Constants::PAYFEE_PAY_ID, QString("='%1'").arg(pay.id()));
        if (query.exec(q->select(Constants::Table_PaymentsFees, where))) {
            while (query.next()) {
                int feeId = query.value(Constants::PAYFEE_FEE_ID).toInt();
                pay.addPaidFeesId(feeId);
                pay.addPaidFee(PaidFee(getFee(QString::number(feeId)), query.value(Constants::PAYFEE_PERCENT_OF_FEE_AMOUNT).toDouble()));
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return pay;
        }
        query.finish();

        // TODO: payment was banked?
        getDates(pay);

        if (transactionCreated) {
            q->database().commit();
            _transaction = false;
        }
        return pay;
    }

    Banking getBanking(const QString &uuid)
    {
        Banking bkg;
        if (!connectDatabase(q->database(), __LINE__))
            return bkg;
        bool transactionCreated = false;
        if (!_transaction) {
            q->database().transaction();
            transactionCreated = true;
            _transaction = true;
        }
        QHash<int, QString> where;
        // Get Banking table
        where.insert(Constants::BANKING_ID, QString("='%1'").arg(uuid));
        QSqlQuery query(q->database());
        if (query.exec(q->select(Constants::Table_Banking, where))) {
            if (query.next()) {
                bkg.setId(query.value(Constants::BANKING_ID).toInt());
                bkg.setValid(query.value(Constants::BANKING_ISVALID).toBool());
                bkg.setBankAccountId(query.value(Constants::BANKING_BANKDETAILS_ID).toInt());
                bkg.setTotalAmount(query.value(Constants::BANKING_TOTAL_AMOUNT).toDouble());
                bkg.setBankingBkid(query.value(Constants::BANKING_BKID).toInt());
                bkg.setDateDid(query.value(Constants::BANKING_DATES_DID).toInt());
                bkg.setSignatureId(query.value(Constants::BANKING_SIGNATURE_ID).toInt());
                bkg.setUuid(query.value(Constants::BANKING_ID).toString());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return bkg;
        }
        query.finish();

        // Get Banking payments from BankingLink table
        where.clear();
        where.insert(Constants::BANKINGLK_BKID, QString("='%1'").arg(bkg.bankingBkid()));
        if (query.exec(q->select(Constants::Table_BankingLink, where))) {
            while (query.next()) {
                int payId = query.value(Constants::BANKINGLK_PAYMENT_ID).toInt();
                bkg.addPaymentsId(payId);
                bkg.addPayment(getPayment(QString::number(payId)));
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return bkg;
        }
        query.finish();

        getDates(bkg);

        if (transactionCreated) {
            q->database().commit();
            _transaction = false;
        }
        return bkg;
    }

    // Retrieve a specific quotation. Creates its own transaction if needed.
    Quotation getQuotation(const QString &uuid)
    {
        Quotation quot;
        if (!connectDatabase(q->database(), __LINE__))
            return quot;
        bool transactionCreated = false;
        if (!_transaction) {
            q->database().transaction();
            transactionCreated = true;
            _transaction = true;
        }
        QHash<int, QString> where;
        // Get Quotation table
        where.insert(Constants::QUOTATION_ID, QString("='%1'").arg(uuid));
        QSqlQuery query(q->database());
        if (query.exec(q->select(Constants::Table_Quotation, where))) {
            if (query.next()) {
                quot.setId(query.value(Constants::QUOTATION_ID).toInt());
                quot.setValid(query.value(Constants::QUOTATION_ISVALID).toBool());
                quot.setCreatorUuid(query.value(Constants::QUOTATION_CREATOR_UID).toString());
                quot.setLabel(query.value(Constants::QUOTATION_LABEL).toString());
                quot.setUserComment(query.value(Constants::QUOTATION_EXTRA_TXT).toString());
                quot.setDateDid(query.value(Constants::QUOTATION_DATE_DID).toInt());
                quot.setTotalAmount(query.value(Constants::QUOTATION_TOTAL_AMOUNT).toInt());
                quot.setSignatureId(query.value(Constants::QUOTATION_SIGNATURE_ID).toInt());
                quot.setUuid(query.value(Constants::QUOTATION_ID).toString());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return quot;
        }
        query.finish();

        // Get Quotation Fees from QuotationLink table
        where.clear();
        where.insert(Constants::QUOTATIONLINK_QUOT_ID, QString("='%1'").arg(uuid));
        if (query.exec(q->select(Constants::Table_QuotationLink, where))) {
            while (query.next()) {
                int feeId = query.value(Constants::QUOTATIONLINK_FEES_ID).toInt();
                quot.addFeesId(feeId);
                quot.addFee(getFee(QString::number(feeId)));
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return quot;
        }
        query.finish();

        // Get Quotation Dates
        getDates(quot);

        if (transactionCreated) {
            q->database().commit();
            _transaction = false;
        }
        return quot;
    }

    // Retrieve all date related to an item. Does not create its transaction.
    // The dateDid() must return a real value.
    bool getDates(VariableDatesItem &item)
    {
        if (item.dateDid()==-1)
            return false;
        if (!connectDatabase(q->database(), __LINE__))
            return false;
        QHash<int, QString> where;
        where.insert(Constants::DATE_DID, QString("='%1'").arg(item.dateDid()));
        QSqlQuery query(q->database());
        if (query.exec(q->select(Constants::Table_Dates, where))) {
            while (query.next()) {
                item.setDate(VariableDatesItem::dateTypeFromSql(query.value(Constants::DATE_TYPE).toString()), query.value(Constants::DATE_ISODATE).toDateTime());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            q->database().rollback();
            return false;
        }
        return true;
    }

    bool getUniqueObject(const AccountBaseQuery &query, AccountBaseResult &result)
    {
        switch (query.retrieveObject()) {
        case AccountBaseQuery::Fee:
            result.addFee(getFee(query.retrieveObjectUuid()));
            return true;
        case AccountBaseQuery::Payment:
            result.addPayment(getPayment(query.retrieveObjectUuid()));
            return true;
        case AccountBaseQuery::Banking:
            result.addBanking(getBanking(query.retrieveObjectUuid()));
            return true;
        case AccountBaseQuery::Quotation:
            result.addQuotation(getQuotation(query.retrieveObjectUuid()));
            return true;
        }
        result.setSuccessfull(false);
        result.setErrorMessage("Nothing to retrieve.");
        return false;
    }

    QList<Fee> getFees(const AccountBaseQuery &query)
    {
        QList<Fee> fees;
        if (query.retrieveObjects().testFlag(AccountBaseQuery::Fee)) {
            if (!connectDatabase(q->database(), __LINE__))
                return fees;
            bool transactionCreated = false;
            if (!_transaction) {
                q->database().transaction();
                transactionCreated = true;
                _transaction = true;
            }

            using namespace Account2::Constants;
            // Build conditions
            Utils::Field get(Table_Fees, FEES_ID);
            Utils::JoinList joins;
            joins << Utils::Join(Table_Dates, DATE_DID, Table_Fees, FEES_DATE_DID);
            Utils::FieldList conds;
            // Validity
            if (!query.includeInvalidObjects())
                conds << Utils::Field(Table_Fees, FEES_ISVALID, QString("=1"));
            // Dates
            if (query.startDate().isValid())
                conds << Utils::Field(Table_Dates, DATE_ISODATE, QString(">= '%1'").arg(query.startDate().toString(Qt::ISODate)));
            if (query.endDate().isValid())
                conds << Utils::Field(Table_Dates, DATE_ISODATE, QString("<= '%1'").arg(query.endDate().toString(Qt::ISODate)));
            // Users
            if (query.userUids().isEmpty()) {
                conds << Utils::Field(Table_Fees, FEES_USER_UID, QString("= '%1'").arg(user()->uuid()));
            } else {
                QString uids;
                if (query.userUids().count() == 1 && query.userUids().at(0) == "%") {
                    conds << Utils::Field(Table_Fees, FEES_USER_UID, QString("LIKE '%'"));
                } else {
                    uids = "'" + query.userUids().join("', '") + "'";
                    conds << Utils::Field(Table_Fees, FEES_USER_UID, QString("IN (%1)").arg(uids));
                }
            }
            // Patients
            if (query.patientUids().isEmpty()) {
                conds << Utils::Field(Table_Fees, FEES_PATIENT_UID, QString("= '%1'").arg(patient()->uuid()));
            } else {
                QString uids;
                if (query.patientUids().count() == 1 && query.patientUids().at(0) == "%") {
                    conds << Utils::Field(Table_Fees, FEES_PATIENT_UID, QString("LIKE '%'"));
                } else {
                    uids = "'" + query.patientUids().join("', '") + "'";
                    conds << Utils::Field(Table_Fees, FEES_PATIENT_UID, QString("IN (%1)").arg(uids));
                }
            }
            QSqlQuery query(q->database());
            if (query.exec(q->select(get, joins, conds))) {
                while (query.next()) {
                    fees << getFee(query.value(0).toString());
                }
            } else {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                // q->database().rollback();
            }
            query.finish();

            if (transactionCreated) {
                q->database().commit();
                _transaction = false;
            }
        }
        return fees;
    }

    QList<Payment> getPayments(const AccountBaseQuery &query)
    {
        QList<Payment> payments;
        if (query.retrieveObjects().testFlag(AccountBaseQuery::Payment)) {
            if (!connectDatabase(q->database(), __LINE__))
                return payments;
            bool transactionCreated = false;
            if (!_transaction) {
                q->database().transaction();
                transactionCreated = true;
                _transaction = true;
            }

            using namespace Account2::Constants;
            // Build conditions
            Utils::Field get(Table_Payments, PAYMENT_ID);
            Utils::JoinList joins;
            joins << Utils::Join(Table_PaymentsFees, PAYFEE_PAY_ID, Table_Payments, PAYMENT_ID);
            joins << Utils::Join(Table_Fees, FEES_ID, Table_PaymentsFees, PAYFEE_FEE_ID);
            joins << Utils::Join(Table_Dates, DATE_DID, Table_Payments, PAYMENT_DATE_DID);
            Utils::FieldList conds;
            // Validity
            if (!query.includeInvalidObjects())
                conds << Utils::Field(Table_Payments, PAYMENT_ISVALID, QString("=1"));
            // Dates
            if (query.startDate().isValid())
                conds << Utils::Field(Table_Dates, DATE_ISODATE, QString(">= '%1'").arg(query.startDate().toString(Qt::ISODate)));
            if (query.endDate().isValid())
                conds << Utils::Field(Table_Dates, DATE_ISODATE, QString("<= '%1'").arg(query.endDate().toString(Qt::ISODate)));
            // Users
            if (query.userUids().isEmpty()) {
                conds << Utils::Field(Table_Fees, FEES_USER_UID, QString("= '%1'").arg(user()->uuid()));
            } else {
                QString uids;
                if (query.userUids().count() == 1 && query.userUids().at(0) == "%") {
                    conds << Utils::Field(Table_Fees, FEES_USER_UID, QString("LIKE '%'"));
                } else {
                    uids = "'" + query.userUids().join("', '") + "'";
                    conds << Utils::Field(Table_Fees, FEES_USER_UID, QString("IN (%1)").arg(uids));
                }
            }
            // Patients
            if (query.patientUids().isEmpty()) {
                conds << Utils::Field(Table_Fees, FEES_PATIENT_UID, QString("= '%1'").arg(patient()->uuid()));
            } else {
                QString uids;
                if (query.patientUids().count() == 1 && query.patientUids().at(0) == "%") {
                    conds << Utils::Field(Table_Fees, FEES_PATIENT_UID, QString("LIKE '%'"));
                } else {
                    uids = "'" + query.patientUids().join("', '") + "'";
                    conds << Utils::Field(Table_Fees, FEES_PATIENT_UID, QString("IN (%1)").arg(uids));
                }
            }

            QSqlQuery query(q->database());
            if (query.exec(q->select(get, joins, conds))) {
                while (query.next()) {
                    payments << getPayment(query.value(0).toString());
                }
            } else {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                // q->database().rollback();
            }
            query.finish();

            if (transactionCreated) {
                q->database().commit();
                _transaction = false;
            }
        }
        return payments;
    }

    QList<Banking> getBankings(const AccountBaseQuery &query)
    {
        QList<Banking> bankings;
        if (query.retrieveObjects().testFlag(AccountBaseQuery::Banking)) {
            if (!connectDatabase(q->database(), __LINE__))
                return bankings;
            bool transactionCreated = false;
            if (!_transaction) {
                q->database().transaction();
                transactionCreated = true;
                _transaction = true;
            }

            using namespace Account2::Constants;
            // Build conditions
            Utils::Field get(Table_Banking, BANKING_ID);
            Utils::JoinList joins;
            joins << Utils::Join(Table_BankingLink, BANKINGLK_BKID, Table_Banking, BANKING_BKID);
            joins << Utils::Join(Table_Payments, PAYMENT_ID, Table_BankingLink, BANKINGLK_PAYMENT_ID);
            joins << Utils::Join(Table_Dates, DATE_DID, Table_Banking, BANKING_DATES_DID);
            joins << Utils::Join(Table_BankDetails, BANKDETAILS_ID, Table_Banking, BANKING_BANKDETAILS_ID);
            Utils::FieldList conds;
            // Validity
            if (!query.includeInvalidObjects())
                conds << Utils::Field(Table_Banking, BANKING_ISVALID, QString("=1"));
            // Dates
            if (query.startDate().isValid())
                conds << Utils::Field(Table_Dates, DATE_ISODATE, QString(">= '%1'").arg(query.startDate().toString(Qt::ISODate)));
            if (query.endDate().isValid())
                conds << Utils::Field(Table_Dates, DATE_ISODATE, QString("<= '%1'").arg(query.endDate().toString(Qt::ISODate)));
            // Users
            if (query.userUids().isEmpty()) {
                conds << Utils::Field(Table_BankDetails, BANKDETAILS_USER_UID, QString("= '%1'").arg(user()->uuid()));
            } else {
                QString uids;
                uids = "'" + query.userUids().join("', '") + "'";
                conds << Utils::Field(Table_BankDetails, BANKDETAILS_USER_UID, QString("IN (%1)").arg(uids));
            }

            QSqlQuery query(q->database());
            if (query.exec(q->select(get, joins, conds))) {
                while (query.next()) {
                    bankings << getBanking(query.value(0).toString());
                }
            } else {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                // q->database().rollback();
            }
            query.finish();

            if (transactionCreated) {
                q->database().commit();
                _transaction = false;
            }
        }
        return bankings;
    }

    bool getMultipleObjects(const AccountBaseQuery &query, AccountBaseResult &result)
    {
        if (query.retrieveObjects().testFlag(AccountBaseQuery::Fee))
            result.setFees(getFees(query));
        if (query.retrieveObjects().testFlag(AccountBaseQuery::Payment))
            result.setPayments(getPayments(query));
        if (query.retrieveObjects().testFlag(AccountBaseQuery::Banking))
            result.setBankings(getBankings(query));
        return true;
    }

    // Save all dates of an item into the database (save or update)
    bool saveDates(VariableDatesItem &item)
    {
        if (!connectDatabase(q->database(), __LINE__))
            return false;
        bool transactionCreated = false;
        if (!_transaction) {
            q->database().transaction();
            transactionCreated = true;
            _transaction = true;
        }

        QSqlQuery query(q->database());
        if (item.dateDid() != -1) {
            // Update -> Delete all previously saved dates
            QHash<int, QString> where;
            where.insert(Constants::DATE_DID, QString("='%1'").arg(item.dateDid()));
            q->prepareDeleteQuery(Constants::Table_Dates, where);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                q->database().rollback();
                return false;
            }
        } else {
            // Save new dates -> get the date DID
            item.setDateDid(q->max(Constants::Table_Dates, Constants::DATE_DID).toInt() + 1);
        }
        for(int i = 0; i < VariableDatesItem::Date_MaxParam; ++i) {
            const QDateTime &dt = item.date(VariableDatesItem::DateType(i));
            if (dt.isNull() || !dt.isValid())
                continue;
            QString req = q->prepareInsertQuery(Constants::Table_Dates);
            query.prepare(req);
            query.bindValue(Constants::DATE_ID, QVariant());
            query.bindValue(Constants::DATE_DID, item.dateDid());
            query.bindValue(Constants::DATE_TYPE, VariableDatesItem::dateTypeToSql(VariableDatesItem::DateType(i)));
            query.bindValue(Constants::DATE_ISODATE, dt);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                q->database().rollback();
                return false;
            }
            query.finish();
        }
        query.finish();

        if (transactionCreated) {
            q->database().commit();
            _transaction = false;
        }
        return true;
    }

    // Save Banking linked Payments and store the linkId in the object
    bool saveBankingPayments(Banking &bkg)
    {
        if (!connectDatabase(q->database(), __LINE__))
            return false;
        bool transactionCreated = false;
        if (!_transaction) {
            q->database().transaction();
            transactionCreated = true;
            _transaction = true;
        }

        QSqlQuery query(q->database());
        if (bkg.bankingBkid() != -1) {
            // Update -> Delete all previously saved links
            QHash<int, QString> where;
            where.insert(Constants::BANKINGLK_BKID, QString("='%1'").arg(bkg.bankingBkid()));
            q->prepareDeleteQuery(Constants::Table_BankingLink, where);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                q->database().rollback();
                return false;
            }
        } else {
            // Save new bankinglinks -> get the ID
            bkg.setBankingBkid(q->max(Constants::Table_Banking, Constants::BANKING_BKID).toInt() + 1);
        }
        for(int i = 0; i < bkg.paymentsId().count(); ++i) {
            int payId = bkg.paymentsId().at(i);
            if (payId == -1)
                continue;
            QString req = q->prepareInsertQuery(Constants::Table_BankingLink);
            query.prepare(req);
            query.bindValue(Constants::BANKINGLK_BKID, bkg.bankingBkid());
            query.bindValue(Constants::BANKINGLK_PAYMENT_ID, payId);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                q->database().rollback();
                return false;
            }
            query.finish();
        }
        query.finish();

        if (transactionCreated) {
            q->database().commit();
            _transaction = false;
        }
        return true;
    }

public:
    bool m_LogChrono;
    bool m_initialized, _transaction;
    Utils::Randomizer r;

private:
    AccountBase *q;
};
}  // End Internal
}  // End Account2

/**
   \brief Constructor.
   \private
*/
AccountBase::AccountBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    d(new AccountBasePrivate(this))
{
    setObjectName("AccountBase");

    // TODO add link table payment <-> quotation
    using namespace Account2::Constants;
    addTable(Table_MedicalProcedure,"MEDPROC");
    addTable(Table_BankDetails,     "BANKACC");
    addTable(Table_Fees,            "FEE");
    addTable(Table_Payments,        "PAY");
    addTable(Table_PaymentsFees,    "PAYFEE");
    addTable(Table_Banking,         "BKG");
    addTable(Table_BankingLink,     "BKGLK");
    addTable(Table_Quotation,       "QUOT");
    addTable(Table_QuotationLink,   "QUOTLNK");
    addTable(Table_Dates,           "DATES");
    addTable(Table_Signatures,      "SIGN");
    addTable(Table_SignatureLink,   "SIGNLINK");

    addTable(Table_VERSION,         "VERSION");
    addField(Table_VERSION, VERSION_ACTUAL, "ACTUAL", FieldIsShortText);

    addField(Table_MedicalProcedure, MP_ID,             "ID",             FieldIsUniquePrimaryKey);
    addField(Table_MedicalProcedure, MP_UID,            "UUID",           FieldIsUUID);
    addField(Table_MedicalProcedure, MP_COUNTRY,        "COUNTRY",        FieldIsLanguageText);
    addField(Table_MedicalProcedure, MP_CATEGORY_ID,    "CATEGORY_ID",    FieldIsInteger);
    addField(Table_MedicalProcedure, MP_CATEGORY_UID,   "CATEGORY_UID",   FieldIsUUID);
    addField(Table_MedicalProcedure, MP_LABEL,          "LABEL",          FieldIsShortText);
    addField(Table_MedicalProcedure, MP_ABSTRACT,       "ABSTRACT",       FieldIsLongText);
    addField(Table_MedicalProcedure, MP_TYPE,           "TYPE",           FieldIsShortText);
    addField(Table_MedicalProcedure, MP_AMOUNT,         "AMOUNT",         FieldIsReal);
    addField(Table_MedicalProcedure, MP_REIMBOURSEMENT, "REIMBOURSEMENT", FieldIsReal);
    addField(Table_MedicalProcedure, MP_DATE_DID,       "DATE_DID",       FieldIsInteger);
    addField(Table_MedicalProcedure, MP_OTHERS,         "OTHERS",         FieldIsBlob);

    // TODO: BANKDETAILS_USER_UID field must be not null
    addField(Table_BankDetails,  BANKDETAILS_ID,             "ID",             FieldIsUniquePrimaryKey);
    addField(Table_BankDetails,  BANKDETAILS_USER_UID,       "USER_UID",       FieldIsUUID);
    addField(Table_BankDetails,  BANKDETAILS_LABEL,          "LABEL",          FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_OWNER,          "OWNER",          FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_OWNERADRESS,    "OWNERADRESS",    FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_ACCOUNTNUMBER,  "ACCNUMB",        FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_IBAN,           "IBAN",           FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_COMMENT,        "COMMENT",        FieldIsLongText);
    addField(Table_BankDetails,  BANKDETAILS_DEFAULT,        "ISDEFAULT",      FieldIsBoolean);

    addField(Table_Fees,  FEES_ID,          "ID",           FieldIsUniquePrimaryKey);
    addField(Table_Fees,  FEES_ISVALID,     "ISVALID",      FieldIsBoolean);
    addField(Table_Fees,  FEES_USER_UID,    "USER_UID",     FieldIsUUID);
    addField(Table_Fees,  FEES_PATIENT_UID, "PATIENT_UID",  FieldIsUUID);
    addField(Table_Fees,  FEES_MP_ID,       "MP_ID",        FieldIsInteger);
    addField(Table_Fees,  FEES_TYPE,        "TYPE",         FieldIsShortText);
    addField(Table_Fees,  FEES_DATE_DID,    "DATE_DID",     FieldIsInteger);
    addField(Table_Fees,  FEES_LABEL,       "LBL",          FieldIsShortText);
    addField(Table_Fees,  FEES_AMOUNT,      "AMOUNT",       FieldIsReal);
    addField(Table_Fees,  FEES_COMMENT,     "COMMENT",      FieldIsShortText);
    addField(Table_Fees,  FEES_SIGN_ID,     "SIGN_ID",      FieldIsInteger);

    addField(Table_Payments, PAYMENT_ID ,       "ID",       FieldIsUniquePrimaryKey);
    addField(Table_Payments, PAYMENT_QUOT_ID ,  "QUOT_ID",  FieldIsInteger);
    addField(Table_Payments, PAYMENT_ISVALID ,  "ISVALID",  FieldIsBoolean);
    addField(Table_Payments, PAYMENT_AMOUNT ,   "AMOUNT",   FieldIsReal);
    addField(Table_Payments, PAYMENT_TYPE ,     "TYPE",     FieldIsShortText);
    addField(Table_Payments, PAYMENT_DATE_DID , "DATE_DID", FieldIsInteger);
    addField(Table_Payments, PAYMENT_COMMENT ,  "COMMENT",  FieldIsShortText);
    addField(Table_Payments, PAYMENT_SIGN_ID ,  "SIGN_ID", FieldIsInteger);

    addField(Table_PaymentsFees, PAYFEE_PAY_ID ,  "PAY_ID",  FieldIsInteger);
    addField(Table_PaymentsFees, PAYFEE_FEE_ID ,  "FEE_ID",  FieldIsInteger);
    addField(Table_PaymentsFees, PAYFEE_PERCENT_OF_FEE_AMOUNT ,  "FEE_AMOUNT_PERCENT",  FieldIsReal);


    addField(Table_Banking, BANKING_ID,             "ID",           FieldIsUniquePrimaryKey);
    addField(Table_Banking, BANKING_ISVALID,        "ISVALID",      FieldIsBoolean);
    addField(Table_Banking, BANKING_BANKDETAILS_ID, "BANKDET_ID",   FieldIsInteger);
    addField(Table_Banking, BANKING_TOTAL_AMOUNT,   "TOTAL",        FieldIsReal);
    addField(Table_Banking, BANKING_BKID,           "BK_ID",        FieldIsInteger);
    addField(Table_Banking, BANKING_DATES_DID,      "DATE_DID",     FieldIsInteger);
    addField(Table_Banking, BANKING_SIGNATURE_ID,   "SIGN_ID",      FieldIsInteger);

    addField(Table_BankingLink, BANKINGLK_BKID ,        "BKID",     FieldIsInteger);
    addField(Table_BankingLink, BANKINGLK_PAYMENT_ID ,  "PAYID",    FieldIsInteger);

    addField(Table_Dates, DATE_ID,      "ID",       FieldIsUniquePrimaryKey);
    addField(Table_Dates, DATE_DID,     "DID",      FieldIsInteger);
    addField(Table_Dates, DATE_TYPE,    "TYPE",     FieldIsShortText);
    addField(Table_Dates, DATE_ISODATE, "ISODATE",  FieldIsDateTime);

    addField(Table_Quotation, QUOTATION_ID,             "ID",           FieldIsUniquePrimaryKey);
    addField(Table_Quotation, QUOTATION_ISVALID,        "ISVALID",      FieldIsBoolean);
    addField(Table_Quotation, QUOTATION_CREATOR_UID,    "CREATOR_UID",  FieldIsUUID);
    addField(Table_Quotation, QUOTATION_LABEL,          "LABEL",        FieldIsShortText);
    addField(Table_Quotation, QUOTATION_EXTRA_TXT,      "EXTRA_TXT",    FieldIsLongText);
    addField(Table_Quotation, QUOTATION_DATE_DID,       "DATE_DID",     FieldIsInteger);
    addField(Table_Quotation, QUOTATION_TOTAL_AMOUNT,   "TOTAL",        FieldIsReal);
    addField(Table_Quotation, QUOTATION_SIGNATURE_ID,   "SIGN_ID",      FieldIsInteger);

    addField(Table_QuotationLink, QUOTATIONLINK_QUOT_ID,   "QUOT_ID",      FieldIsInteger);
    addField(Table_QuotationLink, QUOTATIONLINK_FEES_ID,   "FEES_ID",      FieldIsInteger);

    addField(Table_Signatures, SIGNATURE_ID,        "ID",       FieldIsUniquePrimaryKey);
    addField(Table_Signatures, SIGNATURE_USER_UID,  "USER_UID", FieldIsUUID);
    addField(Table_Signatures, SIGNATURE_DATE_DID,  "DATE_DID", FieldIsInteger);
    addField(Table_Signatures, SIGNATURE_ISVALID,   "ISVALID",  FieldIsBoolean);
    addField(Table_Signatures, SIGNATURE_COMMENT,   "COMMENT",  FieldIsLongText);

    addField(Table_SignatureLink, SIGNATURELINK_LINKID,  "SLINK_ID",    FieldIsInteger);
    addField(Table_SignatureLink, SIGNATURELINK_SIGN_ID, "SIGN_ID",     FieldIsInteger);


    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

/** \brief Destructor. */
AccountBase::~AccountBase()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    if (d)
        delete d;
    d=0;
}

bool AccountBase::initialize()
{
    // only one base can be initialized
    if (d->m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Constants::DB_ACCOUNTANCY, Constants::DB_ACCOUNTANCY,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Constants::DB_ACCOUNTANCY, Constants::DB_ACCOUNTANCY,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ACCOUNTANCY).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR("Wrong database schema");
        return false;
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    d->m_initialized = true;
    return true;
}

bool AccountBase::isInitialized() const
{
    return d->m_initialized;
}

void AccountBase::logChronos(bool log)
{
    d->m_LogChrono = log;
}

AccountBaseResult AccountBase::query(const AccountBaseQuery &query)
{
    AccountBaseResult result;
    if (query.retrieveObject() != AccountBaseQuery::None && !query.retrieveObjectUuid().isEmpty()) {
        // Unique object
        d->getUniqueObject(query, result);
    } else {
        // Multiple objects
        if (!d->getMultipleObjects(query, result))
            LOG_ERROR("Error when retrieving multiple objects");
    }
    return result;
}

bool AccountBase::createVirtuals(int nb)
{
    if (max(Constants::Table_BankDetails, Constants::BANKDETAILS_ID).toInt() < 1) {
        QSqlQuery query(database());
        query.prepare(prepareInsertQuery(Constants::Table_BankDetails));
        query.bindValue(Constants::BANKDETAILS_ID, QVariant());
        query.bindValue(Constants::BANKDETAILS_USER_UID, user()->uuid());
        query.bindValue(Constants::BANKDETAILS_LABEL, d->r.randomWords(d->r.randomInt(0, 10)));
        query.bindValue(Constants::BANKDETAILS_OWNER, d->r.getRandomName());
        query.bindValue(Constants::BANKDETAILS_OWNERADRESS, d->r.randomWords(d->r.randomInt(0, 10)));
        query.bindValue(Constants::BANKDETAILS_ACCOUNTNUMBER, d->r.getRandomString(16));
        query.bindValue(Constants::BANKDETAILS_IBAN, d->r.getRandomString(16));
        query.bindValue(Constants::BANKDETAILS_COMMENT, d->r.randomWords(d->r.randomInt(0, 10)));
        query.bindValue(Constants::BANKDETAILS_DEFAULT, 1);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
        }
    }
    QList<Fee> fees;
    QList<Payment> payments;
    for(int i=0; i < nb; ++i) {
        Fee fee;
        // fee.setId();
        fee.setValid(d->r.randomBool());
        fee.setPatientUuid("all");
        fee.setUserUuid("all");
        fee.setType(d->r.getRandomString(d->r.randomInt(10)));
        fee.setAmount(d->r.randomDouble(10., 1000.));
        fee.setLabel(d->r.randomWords(d->r.randomInt(10)));
        fee.setComment(d->r.randomWords(d->r.randomInt(20)));
        fee.setDate(VariableDatesItem::Date_Creation, d->r.randomDateTime(QDateTime::currentDateTime().addDays(-5)));
        fees << fee;
    }

    save(fees);
    for(int i=0; i < nb; ++i) {
        Payment pay;
        // pay.setId();
        pay.setValid(d->r.randomBool());
        pay.setType(Payment::PaymentType(d->r.randomInt(0, Payment::Other)));
        pay.setDate(VariableDatesItem::Date_Creation, d->r.randomDateTime(QDateTime::currentDateTime().addDays(-5)));
        double percent = d->r.randomDouble(30., 100.);
        pay.addPaidFee(PaidFee(fees.at(i), percent));
        pay.setAmount(fees.at(i).amount() * (percent / 100.));
        payments << pay;
    }
    save(payments);

    Banking bkg;
    bkg.setDate(VariableDatesItem::Date_Creation, d->r.randomDateTime(QDateTime::currentDateTime()));
    for(int i=0; i < payments.count(); ++i) {
        if (d->r.randomBool() && d->r.randomBool() && d->r.randomBool())
            bkg.addPayment(payments.at(i));
    }
    bkg.computeTotalAmount();
    save(bkg);
    return true;
}

bool AccountBase::save(Fee &fee)
{
    return save(QList<Fee>() << fee);
}

bool AccountBase::save(Payment &payment)
{
    return save(QList<Payment>() << payment);
}

bool AccountBase::save(Banking &banking)
{
    return save(QList<Banking>() << banking);
}

bool AccountBase::save(Quotation &quotation)
{
    return save(QList<Quotation>() << quotation);
}

/** Save or update a list of fees */
bool AccountBase::save(QList<Fee> &fees)
{
    if (!connectDatabase(database(), __LINE__))
        return false;
    if (fees.isEmpty())
        return true;

    database().transaction();
    d->_transaction = true;

    QSqlQuery query(database());
    for(int i=0; i < fees.count(); ++i) {
        Fee &fee = fees[i];
        d->saveDates(fee);
        if (fee.id() == -1) {
            // Save
            QString req = prepareInsertQuery(Constants::Table_Fees);
            query.prepare(req);
            query.bindValue(Constants::FEES_ID, QVariant());
            query.bindValue(Constants::FEES_ISVALID, int(fee.isValid()));
            query.bindValue(Constants::FEES_USER_UID, fee.userUid());
            query.bindValue(Constants::FEES_PATIENT_UID, fee.patientUid());
            query.bindValue(Constants::FEES_MP_ID, fee.mpId());
            query.bindValue(Constants::FEES_TYPE, fee.type());
            query.bindValue(Constants::FEES_DATE_DID, fee.dateDid());
            query.bindValue(Constants::FEES_LABEL, fee.label());
            query.bindValue(Constants::FEES_AMOUNT, fee.amount());
            query.bindValue(Constants::FEES_COMMENT, fee.comment());
            query.bindValue(Constants::FEES_SIGN_ID, fee.signatureId());
            if (query.exec()) {
                fee.setId(query.lastInsertId().toInt());
                fee.setModified(false);
            } else {
                LOG_QUERY_ERROR(query);
                query.finish();
                database().rollback();
                return false;
            }
            query.finish();
        } else {
            // Update
            if (fee.isModified()) {
                QHash<int, QString> where;
                where.insert(Constants::FEES_ID, QString("='%1'").arg(fee.id()));
                QString req = prepareUpdateQuery(Constants::Table_Fees, QList<int>()
                                                 << Constants::FEES_ISVALID
                                                 << Constants::FEES_USER_UID
                                                 << Constants::FEES_PATIENT_UID
                                                 << Constants::FEES_MP_ID
                                                 << Constants::FEES_TYPE
                                                 << Constants::FEES_LABEL
                                                 << Constants::FEES_AMOUNT
                                                 << Constants::FEES_COMMENT
                                                 << Constants::FEES_SIGN_ID,
                                                 where);
                query.prepare(req);
                int i = 0;
                query.bindValue(i, int(fee.isValid()));
                query.bindValue(++i, fee.userUid());
                query.bindValue(++i, fee.patientUid());
                query.bindValue(++i, fee.mpId());
                query.bindValue(++i, fee.type());
                query.bindValue(++i, fee.label());
                query.bindValue(++i, fee.amount());
                query.bindValue(++i, fee.comment());
                query.bindValue(++i, fee.signatureId());
                if (query.exec()) {
                    fee.setModified(false);
                } else {
                    LOG_QUERY_ERROR(query);
                    query.finish();
                    database().rollback();
                    return false;
                }
                query.finish();
            }
        }
    }
    query.finish();
    database().commit();
    d->_transaction = false;
    return true;
}

/**
 * Save or update a list of payments.
 * WARNING: all included Fees MUST BE ALREADY SAVED into database.
 */
bool AccountBase::save(QList<Payment> &payments)
{
    if (!connectDatabase(database(), __LINE__))
        return false;
    if (payments.isEmpty())
        return true;

    database().transaction();
    d->_transaction = true;

    QSqlQuery query(database());
    for(int i=0; i < payments.count(); ++i) {
        Payment &pay = payments[i];
        d->saveDates(pay);
        // Save payment
        if (pay.id() == -1) {
            // Save
            QString req = prepareInsertQuery(Constants::Table_Payments);
            query.prepare(req);
            query.bindValue(Constants::PAYMENT_ID, QVariant());
            query.bindValue(Constants::PAYMENT_QUOT_ID, pay.quotationId());
            query.bindValue(Constants::PAYMENT_ISVALID, int(pay.isValid()));
            query.bindValue(Constants::PAYMENT_AMOUNT, pay.amount());
            query.bindValue(Constants::PAYMENT_TYPE, Payment::typeToSql(pay.type()));
            query.bindValue(Constants::PAYMENT_DATE_DID, pay.dateDid());
            query.bindValue(Constants::PAYMENT_COMMENT, pay.comment());
            query.bindValue(Constants::PAYMENT_SIGN_ID, pay.signatureId());
            if (query.exec()) {
                pay.setId(query.lastInsertId().toInt());
                pay.setModified(false);
            } else {
                LOG_QUERY_ERROR(query);
                query.finish();
                database().rollback();
                return false;
            }
            query.finish();
        } else {
            // Update
            if (pay.isModified()) {
                QHash<int, QString> where;
                where.insert(Constants::PAYMENT_ID, QString("='%1'").arg(pay.id()));
                QString req = prepareUpdateQuery(Constants::Table_Fees, QList<int>()
                                                 << Constants::PAYMENT_QUOT_ID
                                                 << Constants::PAYMENT_ISVALID
                                                 << Constants::PAYMENT_AMOUNT
                                                 << Constants::PAYMENT_TYPE
                                                 << Constants::PAYMENT_DATE_DID
                                                 << Constants::PAYMENT_COMMENT
                                                 << Constants::PAYMENT_SIGN_ID,
                                                 where);
                query.prepare(req);
                int i = 0;
                query.bindValue(i, pay.quotationId());
                query.bindValue(++i, int(pay.isValid()));
                query.bindValue(++i, pay.amount());
                query.bindValue(++i, Payment::typeToSql(pay.type()));
                query.bindValue(++i, pay.dateDid());
                query.bindValue(++i, pay.comment());
                query.bindValue(++i, pay.signatureId());
                if (query.exec()) {
                    pay.setModified(false);
                } else {
                    LOG_QUERY_ERROR(query);
                    query.finish();
                    database().rollback();
                    return false;
                }
                query.finish();
            }
        }
        // Save PaymentsToFees
        for(int i=0; i < pay.paidFees().count(); ++i) {
            const PaidFee &paidFee = pay.paidFees().at(i);
            if (paidFee.feeId() == -1) {
                LOG_ERROR("Fee not saved?");
                continue;
            }
            QString req = prepareInsertQuery(Constants::Table_PaymentsFees);
            query.prepare(req);
            query.bindValue(Constants::PAYFEE_PAY_ID, pay.id());
            query.bindValue(Constants::PAYFEE_FEE_ID, paidFee.feeId());
            query.bindValue(Constants::PAYFEE_PERCENT_OF_FEE_AMOUNT, paidFee.paidPercentage());

            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                query.finish();
                database().rollback();
                return false;
            }
            query.finish();
        }

    }
    query.finish();
    database().commit();
    d->_transaction = false;
    return true;
}

bool AccountBase::save(QList<Banking> &bankings)
{
    if (!connectDatabase(database(), __LINE__))
        return false;
    if (bankings.isEmpty())
        return true;

    database().transaction();
    d->_transaction = true;

    QSqlQuery query(database());
    for(int i=0; i < bankings.count(); ++i) {
        Banking &bkg = bankings[i];
        d->saveDates(bkg);
        d->saveBankingPayments(bkg);
        if (bkg.id() == -1) {
            // Save
            QString req = prepareInsertQuery(Constants::Table_Banking);
            query.prepare(req);
            query.bindValue(Constants::BANKING_ID, QVariant());
            query.bindValue(Constants::BANKING_ISVALID, int(bkg.isValid()));
            query.bindValue(Constants::BANKING_BANKDETAILS_ID, bkg.bankAccountId());
            query.bindValue(Constants::BANKING_TOTAL_AMOUNT, bkg.totalAmount());
            query.bindValue(Constants::BANKING_BKID, bkg.bankingBkid());
            query.bindValue(Constants::BANKING_DATES_DID, bkg.dateDid());
            query.bindValue(Constants::BANKING_SIGNATURE_ID, bkg.signatureId());
            if (query.exec()) {
                bkg.setId(query.lastInsertId().toInt());
                bkg.setModified(false);
            } else {
                LOG_QUERY_ERROR(query);
                query.finish();
                database().rollback();
                return false;
            }
            query.finish();
        } else {
            // Update
            if (bkg.isModified()) {
                QHash<int, QString> where;
                where.insert(Constants::BANKING_ID, QString("='%1'").arg(bkg.id()));
                QString req = prepareUpdateQuery(Constants::Table_Banking, QList<int>()
                                                 << Constants::BANKING_ISVALID
                                                 << Constants::BANKING_BANKDETAILS_ID
                                                 << Constants::BANKING_TOTAL_AMOUNT
                                                 << Constants::BANKING_BKID
                                                 << Constants::BANKING_DATES_DID
                                                 << Constants::BANKING_SIGNATURE_ID,
                                                 where);
                query.prepare(req);
                int i = 0;
                query.bindValue(i, int(bkg.isValid()));
                query.bindValue(++i, bkg.bankAccountId());
                query.bindValue(++i, bkg.totalAmount());
                query.bindValue(++i, bkg.bankingBkid());
                query.bindValue(++i, bkg.dateDid());
                query.bindValue(++i, bkg.signatureId());
                if (query.exec()) {
                    bkg.setModified(false);
                } else {
                    LOG_QUERY_ERROR(query);
                    query.finish();
                    database().rollback();
                    return false;
                }
                query.finish();
            }
        }
    }
    query.finish();
    database().commit();
    d->_transaction = false;
    return true;
}

bool AccountBase::save(QList<Quotation> &quotations)
{
    return true;
}

bool AccountBase::createDatabase(const QString &connectionName ,
                                 const QString &dbName,
                                 const QString &pathOrHostName,
                                 TypeOfAccess ,
                                 AvailableDrivers driver,
                                 const QString & login,
                                 const QString & pass,
                                 const int port,
                                 CreationOption
                                 )
{
    // TODO: manage createOption
    if (connectionName != Constants::DB_ACCOUNTANCY)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
        .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__ACCOUNTANCY_CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__ACCOUNTANCY_CREATOR"))
            QSqlDatabase::removeDatabase("__ACCOUNTANCY_CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()));
        return false;
    }

    // Add version number
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_ACTUAL, Constants::DB_VERSION_NUMBER);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.commit();
        return false;
    }
    query.finish();
    DB.rollback();
    return true;
}

void AccountBase::onCoreDatabaseServerChanged()
{
    d->m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_ACCOUNTANCY)) {
        QSqlDatabase::removeDatabase(Constants::DB_ACCOUNTANCY);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    initialize();
}

void AccountBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

//bool AccountBase::checkIfIsFirstVersion()
//{
//    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
//    if (!connectDatabase(DB, __LINE__))
//        return false;
//    QVariant version;
//    DB.transaction();
//    QSqlQuery qy(DB);
//    QString req = select(Constants::Table_VERSION, Constants::VERSION_ACTUAL);//QString("SELECT %1 FROM %2").arg("ACTUAL","VERSION");
//    if (!qy.exec(req)) {
//        LOG_QUERY_ERROR(qy);
//        qy.finish();
//        DB.rollback();
//        return false;
//    }
//    while (qy.next()) {
//        version = qy.value(0);
//    }
//    qy.finish();
//    DB.commit();
//    if (version == QVariant("0.1")) {
//        LOG(QString("VERSION == 0.1"));
//        return true;
//    }
//    return false;
//}

//QString AccountBase::checkAndReplaceVersionNumber()
//{
//    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
//    if (!connectDatabase(DB, __LINE__))
//        return 0;
//    // if (versionHasChanged())
//    // {
//    DB.transaction();
//    QSqlQuery qy(DB);
//    /*QString req = QString("INSERT INTO %1 (%2) VALUES ('%3')")
//                       .arg("VERSION","ACTUAL",QString(Constants::VERSION_ACTUAL));*/
//    qy.prepare(prepareInsertQuery(Constants::Table_VERSION));
//    qy.bindValue(Constants::VERSION_ACTUAL, Constants::DB_VERSION_NUMBER);

//    if (!qy.exec()) {
//        LOG_QUERY_ERROR(qy);
//        qy.finish();
//        DB.rollback();
//        return QString(qy.lastError().text());
//    }
//    qy.finish();
//    DB.commit();
//    return QString(Constants::DB_VERSION_NUMBER);
//    //   }
//}

//bool AccountBase::versionHasChanged()
//{
//    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
//    if (!connectDatabase(DB, __LINE__))
//        return 0;
//    QString version;
//    DB.transaction();
//    QSqlQuery qy(DB);
//    QString req = select(Constants::Table_VERSION, Constants::VERSION_ACTUAL);//QString("SELECT %1 FROM %2").arg("ACTUAL","VERSION");
//    if (!qy.exec(req)) {
//        LOG_QUERY_ERROR(qy);
//        return false;
//    }
//    while (qy.next()) {
//        version = qy.value(0).toString();
//    }
//    qy.finish();
//    DB.commit();
//    if (!version.contains(QString(Constants::DB_VERSION_NUMBER))) {
//        return true;
//    }
//    return false;
//}

//bool AccountBase::checkIfVersionBeforeThirdVersion()
//{
//    QString version;
//    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
//    if (!connectDatabase(DB, __LINE__))
//        return false;
//    DB.transaction();
//    QSqlQuery qy(DB);
//    QString req = select(Constants::Table_VERSION, Constants::VERSION_ACTUAL);//QString("SELECT %1 FROM %2").arg("ACTUAL","VERSION");
//    if (!qy.exec(req))  {
//        LOG_QUERY_ERROR(qy);
//        DB.rollback();
//        return false;
//    }
//    while (qy.next())  {
//        version = qy.value(0).toString();
//    }
//    qy.finish();
//    DB.commit();
//    double versionDouble = version.toDouble();
//    if (versionDouble == 0.0) {
//        LOG("Alter field PATIENT_NAME: Error conversion of double");
//    }
//    if (versionDouble < 0.3) {
//        return true;
//    }
//    return false;
//}

//bool AccountBase::alterFieldPatientNameIntToVarchar()
//{
//    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
//    if (!connectDatabase(DB, __LINE__))
//        return false;
//    QString tableName = table(AccountDB::Constants::Table_Account);
//    QString fieldPatientName    = fieldName(AccountDB::Constants::Table_Account,AccountDB::Constants::ACCOUNT_PATIENT_NAME);
//    QString fieldPatientUuid = fieldName(AccountDB::Constants::Table_Account,AccountDB::Constants::ACCOUNT_PATIENT_UID);
//    if (WarnDebugMessage)
//    qDebug() << __FILE__ << QString::number(__LINE__) << " table + fielName ="
//    << tableName+" + "+fieldPatientName  ;
//    DB.transaction();
//    QSqlQuery qy(DB);
//    QString req = QString("ALTER TABLE %1 MODIFY %2 varchar(2000) NULL , MODIFY %3 varchar(200) NULL ;")
//                         .arg(tableName,fieldPatientName,fieldPatientUuid);
//    if (!qy.exec(req)) {
//        LOG_QUERY_ERROR(qy);
//        qy.finish();
//        DB.rollback();
//        return false;
//    }
//    qy.finish();
//    DB.commit();
//    return true;
//}
