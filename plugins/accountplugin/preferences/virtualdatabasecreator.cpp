/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "virtualdatabasecreator.h"
#include "ui_virtualdatabasecreator.h"

#include <accountbaseplugin/accountbase.h>
#include <accountbaseplugin/constants.h>

#include <accountplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/randomizer.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>

#include <QProgressDialog>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline AccountDB::AccountBase *accountBase() { return AccountDB::AccountBase::instance(); }
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }


static inline void createBankAccount(const QString &userUid, Utils::Randomizer *random)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_BankDetails));
    query.bindValue(AccountDB::Constants::BANKDETAILS_ACCOUNTNUMBER, random->randomString(15));
    query.bindValue(AccountDB::Constants::BANKDETAILS_BALANCE, random->randomInt(-100000, 1239834));
    query.bindValue(AccountDB::Constants::BANKDETAILS_BALANCEDATE, random->randomDateTime(QDateTime::currentDateTime().addDays(-50)).toString(Qt::ISODate));
    query.bindValue(AccountDB::Constants::BANKDETAILS_COMMENT, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_DEFAULT, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_IBAN, random->randomString(15));
    query.bindValue(AccountDB::Constants::BANKDETAILS_ID, QVariant());
    query.bindValue(AccountDB::Constants::BANKDETAILS_OWNER, QString("%1 %2").arg(random->randomName()).arg(random->randomFirstName(1)));
    query.bindValue(AccountDB::Constants::BANKDETAILS_OWNERADRESS, random->randomFrenchCity().second);
    query.bindValue(AccountDB::Constants::BANKDETAILS_LABEL, random->randomString(50));
    query.bindValue(AccountDB::Constants::BANKDETAILS_USER_UID, userUid);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("VirtualDatabaseCreator", query);
    }
    query.finish();
}

static inline void createMedicalProcedure(const QString &userUid, Utils::Randomizer *random)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_MedicalProcedure));
    QStringList abstract;
    for(int i = 0 ; i < random->randomInt(4,20); ++i) {
        abstract << random->randomName();
    }
    query.bindValue(AccountDB::Constants::MP_ABSTRACT, abstract.join(" "));
    query.bindValue(AccountDB::Constants::MP_AMOUNT, random->randomInt(10, 30));
    query.bindValue(AccountDB::Constants::MP_DATE, random->randomDate(2010, 9, 1));
    query.bindValue(AccountDB::Constants::MP_ID, QVariant());
    query.bindValue(AccountDB::Constants::MP_NAME, abstract.join(" "));
    query.bindValue(AccountDB::Constants::MP_REIMBOURSEMENT, QVariant());
    query.bindValue(AccountDB::Constants::MP_TYPE, QVariant());
    query.bindValue(AccountDB::Constants::MP_UID, Utils::Database::createUid());
    query.bindValue(AccountDB::Constants::MP_USER_UID, userUid);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("VirtualDatabaseCreator", query);
    }
    query.finish();
}

static inline void createMovement(const QString &userUid, Utils::Randomizer *random)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_Movement));
    query.bindValue(AccountDB::Constants::MOV_ACCOUNT_ID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_AMOUNT, random->randomInt(10,250));
    query.bindValue(AccountDB::Constants::MOV_AV_MOVEMENT_ID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_COMMENT, "Comment");
    QDate date = random->randomDate(2010, 9, 1);
    query.bindValue(AccountDB::Constants::MOV_DATE, date);
    query.bindValue(AccountDB::Constants::MOV_DATEOFVALUE, random->randomDate(date.year(), date.month(), date.day()));
    query.bindValue(AccountDB::Constants::MOV_DETAILS, "Details");
    query.bindValue(AccountDB::Constants::MOV_ID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_ISVALID, QVariant());
    query.bindValue(AccountDB::Constants::MOV_LABEL, QVariant());
    query.bindValue(AccountDB::Constants::MOV_TRACE, QVariant());
    query.bindValue(AccountDB::Constants::MOV_TYPE, QVariant());
    query.bindValue(AccountDB::Constants::MOV_USER_UID, userUid);
    query.bindValue(AccountDB::Constants::MOV_VALIDITY, QVariant());
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("VirtualDatabaseCreator", query);
    }
    query.finish();
}

static inline void createAccount(const QString &userUid, Utils::Randomizer *random)
{
    QSqlQuery query(accountBase()->database());
    query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_Account));
    query.bindValue(AccountDB::Constants::ACCOUNT_CASHAMOUNT, random->randomInt(100));
    query.bindValue(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT, random->randomInt(100));
    query.bindValue(AccountDB::Constants::ACCOUNT_COMMENT, "Comment");
    QDate now = QDate::currentDate();
    query.bindValue(AccountDB::Constants::ACCOUNT_DATE, random->randomDate(now.year(), 1, 1));
    query.bindValue(AccountDB::Constants::ACCOUNT_DUEAMOUNT, random->randomInt(100));
    query.bindValue(AccountDB::Constants::ACCOUNT_DUEBY, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_ID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT, random->randomInt(100));
    query.bindValue(AccountDB::Constants::ACCOUNT_INSURANCE_ID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_ISVALID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_MEDICALPROCEDURE_TEXT, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_MEDICALPROCEDURE_XML, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_OTHERAMOUNT,random->randomInt(100));
    query.bindValue(AccountDB::Constants::ACCOUNT_PATIENT_NAME, random->randomName());
    query.bindValue(AccountDB::Constants::ACCOUNT_PATIENT_UID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_SITE_ID, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_TRACE, QVariant());
    query.bindValue(AccountDB::Constants::ACCOUNT_UID, Utils::Database::createUid());
    query.bindValue(AccountDB::Constants::ACCOUNT_USER_UID, userUid);
    query.bindValue(AccountDB::Constants::ACCOUNT_VISAAMOUNT, random->randomInt(100));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("VirtualDatabaseCreator", query);
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
QString VirtualDatabaseCreatorPage::displayName() const { return tr("Virtual database creator"); }
QString VirtualDatabaseCreatorPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
QString VirtualDatabaseCreatorPage::title() const {return tr("Accountancy virtual database creator");}
int VirtualDatabaseCreatorPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 90;
}

void VirtualDatabaseCreatorPage::resetToDefaults()
{
}

void VirtualDatabaseCreatorPage::apply()
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

void VirtualDatabaseCreator::setDataToUi()
{
}

void VirtualDatabaseCreator::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
}

void VirtualDatabaseCreator::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    qWarning() << "---------> writedefaults";
    LOG_FOR("VirtualDatabaseCreator", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("VirtualDatabaseCreator"));
//    s->sync();
}

void VirtualDatabaseCreator::on_populate_clicked()
{
    int nb = ui->bankAccounts->value() + ui->medicalProcedures->value() + ui->accounts->value() + ui->movements->value();
    int stepsDone = 0;
    QString userUid = user()->uuid();
//    int userLkId = userModel()->practionnerLkIds(userModel()->currentUserData(Core::IUser::Uuid).toString()).at(0);
    QProgressDialog dlg(tr("Creating virtual database (%1 items to create)").arg(nb), tr("Cancel"), 0, nb, qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles");

    // create bank accounts
    accountBase()->database().transaction();
    for(int i = 0; i < ui->bankAccounts->value(); ++i) {
        dlg.setValue(++stepsDone);
        createBankAccount(userUid, &r);
    }
    accountBase()->database().commit();

    // create medical procedures
    accountBase()->database().transaction();
    for(int i = 0; i < ui->medicalProcedures->value(); ++i) {
        dlg.setValue(++stepsDone);
        createMedicalProcedure(userUid, &r);
    }
    accountBase()->database().commit();

    // create movements
    accountBase()->database().transaction();
    for(int i = 0; i < ui->movements->value(); ++i) {
        dlg.setValue(++stepsDone);
        createMovement(userUid, &r);
    }
    accountBase()->database().commit();

    // create accounts
    accountBase()->database().transaction();
    for(int i = 0; i < ui->accounts->value(); ++i) {
        dlg.setValue(++stepsDone);
        createAccount(userUid, &r);
    }
    accountBase()->database().commit();
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




