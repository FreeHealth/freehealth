/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "virtualdatabasecreator.h"
#include "ui_virtualdatabasecreator.h"

#include <accountbaseplugin/accountbase.h>
#include <accountbaseplugin/constants.h>

#include <accountplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>

#include <QProgressDialog>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline AccountDB::AccountBase *accountBase() { return AccountDB::AccountBase::instance(); }
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }


static inline void createBankAccount(const QString &userUid)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_BankDetails));
    query.bindValue(AccountDB::Constants::BANKDETAILS_ACCOUNTNUMBER, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_BALANCE, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_BALANCEDATE, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_COMMENT, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_DEFAULT, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_IBAN, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_ID, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_OWNER, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_OWNERADRESS, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_LABEL, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_USER_UID, userUid);
    if (!query.exec()) {
        Utils::Log::addQueryError("VirtualDatabaseCreator", query, __FILE__, __LINE__);
    }
    query.finish();
}

static inline void createMedicalProcedure(const QString &userUid)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_MedicalProcedure));
    query.bindValue(AccountDB::Constants::MP_ABSTRACT, QVariant());
    query.bindValue(AccountDB::Constants::MP_AMOUNT, QVariant());
    query.bindValue(AccountDB::Constants::MP_DATE, QVariant());
    query.bindValue(AccountDB::Constants::MP_ID, QVariant());
    query.bindValue(AccountDB::Constants::MP_NAME, QVariant());
    query.bindValue(AccountDB::Constants::MP_REIMBOURSEMENT, QVariant());
    query.bindValue(AccountDB::Constants::MP_TYPE, QVariant());
    query.bindValue(AccountDB::Constants::MP_UID, QVariant());
    query.bindValue(AccountDB::Constants::MP_USER_UID, userUid);
    if (!query.exec()) {
        Utils::Log::addQueryError("VirtualDatabaseCreator", query, __FILE__, __LINE__);
    }
    query.finish();
}

static inline void createMovement(const QString &userUid)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_Movement));
    query.bindValue(AccountDB::Constants::MOV_ACCOUNT_ID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_AMOUNT, QVariant());
    query.bindValue(AccountDB::Constants::MOV_AV_MOVEMENT_ID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_COMMENT, QVariant());
    query.bindValue(AccountDB::Constants::MOV_DATE, QVariant());
    query.bindValue(AccountDB::Constants::MOV_DATEOFVALUE, QVariant());
    query.bindValue(AccountDB::Constants::MOV_DETAILS, QVariant());
    query.bindValue(AccountDB::Constants::MOV_ID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_ISVALID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_LABEL, QVariant());
    query.bindValue(AccountDB::Constants::MOV_TRACE, QVariant());
    query.bindValue(AccountDB::Constants::MOV_TYPE, QVariant());
    query.bindValue(AccountDB::Constants::MOV_USER_UID, userUid);
    query.bindValue(AccountDB::Constants::MOV_VALIDITY, QVariant());
    if (!query.exec()) {
        Utils::Log::addQueryError("VirtualDatabaseCreator", query, __FILE__, __LINE__);
    }
    query.finish();
}

static inline void createAccount(const QString &userUid)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_Account));
    query.bindValue(AccountDB::Constants::ACCOUNT_CASHAMOUNT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_COMMENT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_DATE, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_DUEAMOUNT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_DUEBY, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_ID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_INSURANCE_ID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_ISVALID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_MEDICALPROCEDURE_TEXT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_MEDICALPROCEDURE_XML, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_OTHERAMOUNT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_PATIENT_NAME, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_PATIENT_UID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_SITE_ID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_TRACE, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_UID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_USER_UID, userUid);
    query.bindValue(AccountDB::Constants::ACCOUNT_VISAAMOUNT, QVariant());
    if (!query.exec()) {
        Utils::Log::addQueryError("VirtualDatabaseCreator", query, __FILE__, __LINE__);
    }
    query.finish();
}

static inline void refreshModels()
{
}



VirtualDatabaseCreatorPage::VirtualDatabaseCreatorPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("VirtualDatabaseCreatorPage"); }

VirtualDatabaseCreatorPage::~VirtualDatabaseCreatorPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

QString VirtualDatabaseCreatorPage::id() const { return objectName(); }
QString VirtualDatabaseCreatorPage::name() const { return tr("Virtual database creator"); }
QString VirtualDatabaseCreatorPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void VirtualDatabaseCreatorPage::resetToDefaults()
{
}

void VirtualDatabaseCreatorPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
}

void VirtualDatabaseCreatorPage::finish() { delete m_Widget; }

void VirtualDatabaseCreatorPage::checkSettingsValidity()
{
}

QWidget *VirtualDatabaseCreatorPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new VirtualDatabaseCreator(parent);
    return m_Widget;
}





VirtualDatabaseCreator::VirtualDatabaseCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::VirtualDatabaseCreator)
{
    ui->setupUi(this);
}

VirtualDatabaseCreator::~VirtualDatabaseCreator()
{
    delete ui;
}

void VirtualDatabaseCreator::setDatasToUi()
{
}

void VirtualDatabaseCreator::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
}

void VirtualDatabaseCreator::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("AccountUserWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("VirtualDatabaseCreator"));
    s->sync();
}

void VirtualDatabaseCreator::on_populate_clicked()
{
    int nb = ui->bankAccounts->value() + ui->medicalProcedures->value() + ui->accounts->value() + ui->movements->value();
    int stepsDone = 0;
    QString userUid = user()->value(Core::IUser::Uuid).toString();
//    int userLkId = userModel()->practionnerLkIds(userModel()->currentUserData(Core::IUser::Uuid).toString()).at(0);
    QProgressDialog dlg(tr("Creating virtual database (%1 items to create)").arg(nb), tr("Cancel"), 0, nb, qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // create bank accounts
    accountBase()->database().transaction();
    for(int i = 0; i < ui->bankAccounts->value(); ++i) {
        dlg.setValue(++stepsDone);
//        createBankAccount();
    }
    accountBase()->database().commit();

    // create medical procedures
    accountBase()->database().transaction();
    for(int i = 0; i < ui->medicalProcedures->value(); ++i) {
        dlg.setValue(++stepsDone);
    }
    accountBase()->database().commit();

    // create movements
    accountBase()->database().transaction();
    for(int i = 0; i < ui->movements->value(); ++i) {
        dlg.setValue(++stepsDone);
    }
    accountBase()->database().commit();

    // create accounts
    accountBase()->database().transaction();
    for(int i = 0; i < ui->accounts->value(); ++i) {
        dlg.setValue(++stepsDone);
    }
    accountBase()->database().commit();

//    for(int i = 0; i < nb; ++i) {
//        if (i % 100 == 0) {
//            dlg.setValue(i);
//            patientBase()->database().transaction();
//        }
//        QString name, sur, sec, g;
//        int title, lk;
//        QDate death, dob;

//        name = r.getRandomName();
//        dob = r.randomDate(1910);

//        if (r.randomInt(2) == 1) {
//            g = "F";
//            sur= r.getRandomFirstname(false);
//            sec = r.getRandomName();
//            title = 2;
//        } else {
//            g = "M";
//            sur= r.getRandomFirstname(true);
//            sec = r.getRandomName();
//            title = 1;
//        }

//        if (r.randomInt(2)) {
//            death = r.randomDate(1980);
//        }
//        QPair<int, QString> p = r.getRandomFrenchCity();
//        if (r.randomInt(3) == 1)
//            lk =  userLkId + 1;
//        else
//            lk = userLkId;

//        createPatient(name,sec, sur,g,title,
//                      dob,"France","",r.getRandomString(65),
//                      QString::number(p.first), p.second,
//                      QUuid::createUuid().toString(), lk, "", death);

//        if (i % 100 == 99)
//            patientBase()->database().commit();

//        qApp->processEvents();
//        if (dlg.wasCanceled())
//            break;
//    }
//    patientBase()->database().commit();

//    refreshPatientModel();
}

void VirtualDatabaseCreator::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}




