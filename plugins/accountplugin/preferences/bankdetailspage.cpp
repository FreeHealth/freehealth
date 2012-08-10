/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "bankdetailspage.h"
#include <accountplugin/constants.h>

#include <accountbaseplugin/bankaccountmodel.h>
#include <accountbaseplugin/constants.h>


#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

enum { WarnDebugMessage = false };

using namespace Account;
using namespace Constants;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


BankDetailsPage::BankDetailsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("BankDetailsPage");
}

BankDetailsPage::~BankDetailsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString BankDetailsPage::id() const { return objectName(); }
QString BankDetailsPage::name() const { return tkTr(Trans::Constants::BANK_DETAILS); }
QString BankDetailsPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void BankDetailsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void BankDetailsPage::applyChanges()
{
    if (WarnDebugMessage)
        LOG("applyChanges");
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void BankDetailsPage::finish() { delete m_Widget; }

void BankDetailsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *BankDetailsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new BankDetailsWidget(parent);
    return m_Widget;
}

BankDetailsWidget::BankDetailsWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setObjectName("BankDetailWidget");
    setupUi(this);
    balanceDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    balanceDate->setDate(QDate::currentDate());
    m_user_uid = user()->uuid();
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }
    uidLabel->setText(m_user_uid);
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    removeButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    m_Model = new AccountDB::BankAccountModel(this);
    if (m_Model->rowCount()<1)  {
        if (!setCashBox())  {
            LOG_ERROR("Unable to create cash box.");
        }
    }
    // TODO:  m_Model->setUserUuid();
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(uidLabel,AccountDB::Constants::BANKDETAILS_USER_UID,"text");
    m_Mapper->addMapping(currentLabel, AccountDB::Constants::BANKDETAILS_LABEL, "text");
    m_Mapper->addMapping(owner, AccountDB::Constants::BANKDETAILS_OWNER, "text");
    m_Mapper->addMapping(ownerAdress, AccountDB::Constants::BANKDETAILS_OWNERADRESS, "text");
    m_Mapper->addMapping(iban, AccountDB::Constants::BANKDETAILS_IBAN, "text");
    m_Mapper->addMapping(number, AccountDB::Constants::BANKDETAILS_ACCOUNTNUMBER, "text");
    m_Mapper->addMapping(balanceSpin, AccountDB::Constants::BANKDETAILS_BALANCE, "value");
    m_Mapper->addMapping(balanceDate, AccountDB::Constants::BANKDETAILS_BALANCEDATE, "date");
    m_Mapper->addMapping(defaultCheckBox, AccountDB::Constants::BANKDETAILS_DEFAULT);
    accountComboBox->setModel(m_Model);
    accountComboBox->setModelColumn(AccountDB::Constants::BANKDETAILS_LABEL);
    setDatasToUi();
}

BankDetailsWidget::~BankDetailsWidget()
{
    //saveModel();
}

void BankDetailsWidget::setDatasToUi()
{
    m_Mapper->setCurrentIndex(accountComboBox->currentIndex());
}

void BankDetailsWidget::saveModel()
{
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes?"),
                                          tr("You make changes into the bank account details.\n"
                                             "Do you want to save them?"));
        if (yes) {
            if (!m_Model->submit()) {
                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("bank account details")));
            }
        } else {
            m_Model->revert();
        }
    }
}

void BankDetailsWidget::on_accountComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    m_Mapper->setCurrentIndex(accountComboBox->currentIndex());
}

void BankDetailsWidget::on_addButton_clicked()
{
    m_Model->insertRow(m_Model->rowCount());
    accountComboBox->setCurrentIndex(m_Model->rowCount() - 1);
    uidLabel->setFocus();
    uidLabel->setText(m_user_uid);
    owner->setFocus();
    owner->setText(m_user_fullName);
}

void BankDetailsWidget::on_removeButton_clicked()
{
    m_Model->removeRow(accountComboBox->currentIndex());
    accountComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void BankDetailsWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    if (!m_Model->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("bank account details")));
        Utils::warningMessageBox(tr("Can not submit bank account details to your personnal database."),
                                 tr("An error occured during bank account details saving. Datas are corrupted."));
    }
}

void BankDetailsWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Utils::Log::addMessage("BankDetailsWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("BankDetailsWidget"));
//    s->sync();
}

void BankDetailsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        {
            retranslateUi(this);
            addButton->setToolTip(tkTr(Trans::Constants::FILENEW_TEXT));
            removeButton->setToolTip(tkTr(Trans::Constants::REMOVE_TEXT));
            break;
        }
    default:
        break;
    }
}

bool BankDetailsWidget::setCashBox()
{
    if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex())) {
        LOG_ERROR("Unable to insertRow in model");
    }
    QString label = trUtf8("cash box") ;
    QString comment = trUtf8("Your cash till");
    m_Model->setData(m_Model->index(m_Model->rowCount()-1,AccountDB::Constants::BANKDETAILS_USER_UID),m_user_uid,Qt::EditRole);
    m_Model->setData(m_Model->index(m_Model->rowCount()-1,AccountDB::Constants::BANKDETAILS_LABEL),label,Qt::EditRole);
    m_Model->setData(m_Model->index(m_Model->rowCount()-1,AccountDB::Constants::BANKDETAILS_OWNER),m_user_fullName,Qt::EditRole);
    m_Model->setData(m_Model->index(m_Model->rowCount()-1,AccountDB::Constants::BANKDETAILS_COMMENT),comment,Qt::EditRole);
    return m_Model->submit();
}
