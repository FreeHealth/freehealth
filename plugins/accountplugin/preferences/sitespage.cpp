/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *   pm.desombre@medsyn.fr                                                 *
 *   original from Eric Maeker, MD.                                        *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "sitespage.h"
#include <accountplugin/constants.h>

#include <accountbaseplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>

#include <coreplugin/constants_icons.h>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  AccountUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
SitesPage::SitesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("SitesPage"); }

SitesPage::~SitesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString SitesPage::id() const { return objectName(); }
QString SitesPage::name() const { return tkTr(Trans::Constants::SITES); }
QString SitesPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void SitesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void SitesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void SitesPage::finish() { delete m_Widget; }

void SitesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *SitesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new SitesWidget(parent);
    return m_Widget;
}

SitesWidget::SitesWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setObjectName("SitesWidget");
    setupUi(this);
    m_user_uid = user()->value(Core::IUser::Uuid).toString();
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    addButton->setText("New");
    deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    deleteButton->setText("Delete");
    zipComboBox->setEditable(true);
    zipComboBox->setInsertPolicy(QComboBox::InsertAtTop);
    countryComboBox->setEditable(true);
    countryComboBox->setInsertPolicy(QComboBox::InsertAtTop);
    m_Model = new AccountDB::WorkingPlacesModel(this);
        /** \todo  m_Model->setUserUuid(); */
    QLabel *siteUidLabel = new QLabel(this);
    siteUidLabel->setText("NULL");
    siteUidLabel->hide();
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    //m_Mapper->addMapping(mpIDLabel, AccountDB::Constants::SITE_ID, "ID");
    m_Mapper->addMapping(siteUidLabel, AccountDB::Constants::SITES_UID);
    m_Mapper->addMapping(wpComboBox, AccountDB::Constants::SITES_NAME);
    m_Mapper->addMapping(nameEdit, AccountDB::Constants::SITES_NAME);
    m_Mapper->addMapping(adressEdit, AccountDB::Constants::SITES_ADRESS);
    m_Mapper->addMapping(cityEdit, AccountDB::Constants::SITES_CITY);
    m_Mapper->addMapping(zipComboBox, AccountDB::Constants::SITES_ZIPCODE);
    m_Mapper->addMapping(countryComboBox, AccountDB::Constants::SITES_COUNTRY);
    m_Mapper->addMapping(phoneEdit, AccountDB::Constants::SITES_TEL);
    
    m_Mapper->addMapping(phoneEdit, AccountDB::Constants::SITES_TEL);
    m_Mapper->addMapping(faxEdit, AccountDB::Constants::SITES_FAX);
    m_Mapper->addMapping(mailEdit, AccountDB::Constants::SITES_MAIL);
    
    m_Mapper->addMapping(contactEdit, AccountDB::Constants::SITES_CONTACT);

    wpComboBox->setModel(m_Model);
    wpComboBox->setModelColumn(AccountDB::Constants::SITES_NAME);
    setDatasToUi();
}

SitesWidget::~SitesWidget()
{
    saveModel();
}

void SitesWidget::setDatasToUi()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(wpComboBox->currentIndex());
    m_Mapper->setCurrentIndex(wpComboBox->currentIndex());
}

void SitesWidget::saveModel()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    if (!m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes ?"),
                                          tr("You make changes into the sites table.\n"
                                             "Do you want to save them ?"));
        if (yes) {
            if (!m_Model->submit()) {
                Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("sites")), __FILE__, __LINE__);
            }
        } else {
            m_Model->revert();
        }
    }
}

void SitesWidget::on_wpComboBox_currentIndexChanged(int index)
{
    //saveModel();
    m_Mapper->setCurrentIndex(wpComboBox->currentIndex());
}

void SitesWidget::on_addButton_clicked()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        Utils::Log::addError(this, "Unable to add row", __FILE__, __LINE__);
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    wpComboBox->setCurrentIndex(m_Model->rowCount()-1);
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());

}


void SitesWidget::on_deleteButton_clicked()
{
    if (!m_Model->removeRow(wpComboBox->currentIndex()))
    {
    	  Utils::Log::addError(this, "Unable to remove row", __FILE__, __LINE__);
        }
    wpComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void SitesWidget::saveToSettings(Core::ISettings *sets)
{
    if (!m_Model->submit()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("sites")));
        Utils::warningMessageBox(tr("Can not submit sites to your personnal database."),
                                 tr("An error occured during sites saving. Datas are corrupted."));
    }
        connect(nameEdit,SIGNAL(textEdited(const QString &)),wpComboBox,SLOT(setEditText(const QString &)));
        update();
}

void SitesWidget::writeDefaultSettings(Core::ISettings *s)
{
//    Utils::Log::addMessage("SitesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("SitesWidget"));
//    s->sync();
}

void SitesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
//        int s = defaultCombo->currentIndex();
//        defaultCombo->clear();
//        defaultCombo->addItem(tkTr(Trans::Constants::NO));
//        defaultCombo->addItem(tkTr(Trans::Constants::YES));
//        defaultCombo->setCurrentIndex(s);
        break;
    default:
        break;
    }
}
