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
#ifndef ACCOUNT2_DATABASE_CONSTANTS_H
#define ACCOUNT2_DATABASE_CONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 * \file constants_db.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 13 Mar 2013
*/

namespace Account2 {
namespace Constants {

    const char * const DB_ACCOUNTANCY           = "account2";
    const char * const DEFAULT_ACCOUNTANCY_USER = "AccountancyDefaultUser";
    const char * const DB_VERSION_NUMBER        = "0.1";
    
    //DatapackMPModel
//    const char * const DATAPACK_ACCOUNTANCY     = "account";
//    const char * const DATAPACK_DB              = "mp_datapack";
//    const char * const DATAPACK_ACCOUNTANCY_FILENAME    = "mp_datapack.db";

    // Database abstraction
    
    enum Tables {
        Table_MedicalProcedure = 0,
        Table_BankDetails,
        Table_Fees,
        Table_Payments,
        Table_PaymentsFees,
        Table_Banking,
        Table_BankingLink,
        Table_Quotation,
        Table_QuotationLink,
        Table_Dates,
        Table_Signatures,
        Table_SignatureLink,
//        Table_Deposit,
//        Table_Account,
//        Table_Acts,
//        Table_Assets,
//        Table_AssetsRates,
//        Table_Movement,
//        Table_AvailableMovement,
//        Table_Insurance,
//        Table_Sites,
//        Table_Users,
//        Table_Percent,
//        Table_Rules,
//        Table_DistanceRules,
//        Table_Thesaurus,
//        Table_Session,
//        Table_Banking,
//        Table_NameIndex,
        Table_VERSION
    };
    
    enum TableMedicalProcedure_Fields {
        MP_ID = 0,
        MP_UID,
        MP_COUNTRY,
        MP_CATEGORY_ID,
        MP_CATEGORY_UID,
        MP_LABEL,
        MP_ABSTRACT,
        MP_QUOTATION_EXTRATEXT,
        MP_TYPE,
        MP_AMOUNT,
        MP_REIMBOURSEMENT,
        MP_DATE_DID,  // creation, update, validity
        MP_OTHERS,
        MP_MaxParam
    };

    enum TableBankDetails_Fields {
        BANKDETAILS_ID = 0,
        BANKDETAILS_USER_UID,
        BANKDETAILS_LABEL,
        BANKDETAILS_OWNER,
        BANKDETAILS_OWNERADRESS,
        BANKDETAILS_ACCOUNTNUMBER,
        BANKDETAILS_IBAN,
        BANKDETAILS_COMMENT,
        BANKDETAILS_DEFAULT,
        BANKDETAILS_MaxParam
    };

    enum TableFees_Fields {
        FEES_ID = 0,
        FEES_ISVALID,
        FEES_USER_UID,
        FEES_PATIENT_UID,
        FEES_MP_ID,
        FEES_TYPE,
        FEES_DATE_DID,
        FEES_LABEL,
        FEES_AMOUNT,
        FEES_COMMENT,
        FEES_SIGN_ID,
        FEES_MaxParam
    };

    enum Table_Payments_Fields {
        PAYMENT_ID = 0,
        PAYMENT_QUOT_ID,
        PAYMENT_ISVALID,
        PAYMENT_AMOUNT,
        PAYMENT_TYPE,
        PAYMENT_DATE_DID,
        PAYMENT_COMMENT,
        PAYMENT_SIGN_ID,
        PAYMENT_MaxParam
    };

    enum Table_PaymentsFeesLink_Fields {
        PAYFEE_PAY_ID = 0,
        PAYFEE_FEE_ID
    };

    enum Table_Banking_Fields {
        BANKING_ID = 0,
        BANKING_ISVALID,
        BANKING_BANKDETAILS_ID,
        BANKING_TOTAL_AMOUNT,
        BANKING_BKID,
        BANKING_DATES_DID,
        BANKING_SIGNATURE_ID,
        BANKING_MaxParam
    };

    enum Table_BankingLink_Fields {
        BANKINGLK_BKID = 0,
        BANKINGLK_PAYMENT_ID
    };

    enum Table_Date_Fields {
        DATE_ID = 0,
        DATE_DID,
        DATE_TYPE,
        DATE_ISODATE
    };

    enum Table_Quotation_Fields {
        QUOTATION_ID = 0,
        QUOTATION_ISVALID,
        QUOTATION_CREATOR_UID,
        QUOTATION_LABEL,
        QUOTATION_EXTRA_TXT,
        QUOTATION_DATE_DID,
        QUOTATION_TOTAL_AMOUNT,
        QUOTATION_SIGNATURE_ID
    };

    enum Table_QuotationLink_Fields {
        QUOTATIONLINK_QUOT_ID = 0,
        QUOTATIONLINK_FEES_ID
    };

    enum Table_Signatures_Fields {
        SIGNATURE_ID = 0,
        SIGNATURE_USER_UID,
        SIGNATURE_DATE_DID,
        SIGNATURE_ISVALID,
        SIGNATURE_COMMENT
    };

    enum Table_SignatureLink_Fields {
        SIGNATURELINK_LINKID = 0,
        SIGNATURELINK_SIGN_ID
    };

    enum Table_Version_Fields{
        VERSION_ACTUAL = 0
    };
    
}  // End namespace Constants
}  // End namespace Account2

#endif // ACCOUNTBASE_CONSTANTS_H
