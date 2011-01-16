/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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
#include <coreplugin/constants_icons.h>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
BankDetailsPage::BankDetailsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("BankDetailsPage"); }

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
{qDebug() << __FILE__ << QString::number(__LINE__) << " applyChanges ";
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
    setupUi(this);
    defaultCombo->addItem(tkTr(Trans::Constants::NO));
    defaultCombo->addItem(tkTr(Trans::Constants::YES));
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    removeButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    m_Model = new AccountDB::BankAccountModel(this);
    /** \todo  m_Model->setUserUuid(); */
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(currentLabel, AccountDB::Constants::BANKDETAILS_LABEL, "text");
    m_Mapper->addMapping(owner, AccountDB::Constants::BANKDETAILS_OWNER, "text");
    m_Mapper->addMapping(ownerAdress, AccountDB::Constants::BANKDETAILS_OWNERADRESS, "text");
    m_Mapper->addMapping(iban, AccountDB::Constants::BANKDETAILS_IBAN, "text");
    m_Mapper->addMapping(number, AccountDB::Constants::BANKDETAILS_ACCOUNTNUMBER, "text");
    m_Mapper->addMapping(balanceSpin, AccountDB::Constants::BANKDETAILS_BALANCE, "value");
    m_Mapper->addMapping(balanceDate, AccountDB::Constants::BANKDETAILS_BALANCEDATE, "date");
    m_Mapper->addMapping(defaultCombo, AccountDB::Constants::BANKDETAILS_BALANCEDATE, "currentIndex");
    accountComboBox->setModel(m_Model);
    accountComboBox->setModelColumn(AccountDB::Constants::BANKDETAILS_LABEL);
    setDatasToUi();
}

BankDetailsWidget::~BankDetailsWidget()
{
    saveModel();
}

void BankDetailsWidget::setDatasToUi()
{
    m_Mapper->setCurrentIndex(accountComboBox->currentIndex());
}

void BankDetailsWidget::saveModel()
{
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes ?"),
                                          tr("You make changes into the bank account details.\n"
                                             "Do you want to save them ?"));
        if (yes) {
            if (!m_Model->submit()) {
                Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("bank account details")));
            }
        } else {
            m_Model->revert();
        }
    }
}

void BankDetailsWidget::on_accountComboBox_currentIndexChanged(int index)
{
    saveModel();
    m_Mapper->setCurrentIndex(accountComboBox->currentIndex());
}

void BankDetailsWidget::on_addButton_clicked()
{
    m_Model->insertRow(m_Model->rowCount());
    accountComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void BankDetailsWidget::on_removeButton_clicked()
{
    m_Model->removeRow(accountComboBox->currentIndex());
    accountComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void BankDetailsWidget::saveToSettings(Core::ISettings *sets)
{
    if (!m_Model->submit()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("bank account details")));
        Utils::warningMessageBox(tr("Can not submit bank account details to your personnal database."),
                                 tr("An error occured during bank account details saving. Datas are corrupted."));
    }
}

void BankDetailsWidget::writeDefaultSettings(Core::ISettings *s)
{
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
            int s = defaultCombo->currentIndex();
            defaultCombo->clear();
            defaultCombo->addItem(tkTr(Trans::Constants::NO));
            defaultCombo->addItem(tkTr(Trans::Constants::YES));
            defaultCombo->setCurrentIndex(s);
            break;
        }
    default:
        break;
    }
}
