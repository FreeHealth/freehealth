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
#include "distancerulespage.h"
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

#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QRegExp>
#include <QLocale>
#include <QUuid>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  AccountUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DistanceRulesPage::DistanceRulesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DistanceRulesPage"); }

DistanceRulesPage::~DistanceRulesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DistanceRulesPage::id() const { return objectName(); }
QString DistanceRulesPage::name() const { return tkTr(Trans::Constants::DISTRULES); }
QString DistanceRulesPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void DistanceRulesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void DistanceRulesPage::applyChanges()
{qDebug() << __FILE__ << QString::number(__LINE__) << " applyChanges ";
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void DistanceRulesPage::finish() { delete m_Widget; }

void DistanceRulesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *DistanceRulesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DistanceRulesWidget(parent);
    return m_Widget;
}

DistanceRulesWidget::DistanceRulesWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setObjectName("DistanceRulesWidget");
    setupUi(this);
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    addButton->setText("New");
    deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    deleteButton->setText("Delete");
    
    m_Model = new AccountDB::DistanceRulesModel(this);
        /** \todo  m_Model->setUserUuid(); */
    distanceRulesUidLabel->setText("");
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(distanceRulesUidLabel,AccountDB::Constants::DISTRULES_UID);
    m_Mapper->addMapping(typeEdit, AccountDB::Constants::DISTRULES_TYPE);
    m_Mapper->addMapping(valueDoubleSpinBox, AccountDB::Constants::DISTRULES_VALUES);
    //m_Mapper->toFirst();
    distanceRulesComboBox->setModel(m_Model);
    distanceRulesComboBox->setModelColumn(AccountDB::Constants::DISTRULES_TYPE);
    setDatasToUi();
}

DistanceRulesWidget::~DistanceRulesWidget()
{
    //saveModel();
}

void DistanceRulesWidget::setDatasToUi()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(distanceRulesComboBox->currentIndex());
    m_Mapper->setCurrentIndex(distanceRulesComboBox->currentIndex());
}

void DistanceRulesWidget::saveModel()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes ?"),
                                          tr("You make changes into the distancerules table.\n"
                                             "Do you want to save them ?"));
        if (yes) {
           if (!m_Model->submit()) {qDebug() << __FILE__ << QString::number(__LINE__) << " distancerules no submit ";
                Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("distancerules")), __FILE__, __LINE__);
            }
        } 
        else {
            m_Model->revert();
        }
    }
    qDebug() << __FILE__ << QString::number(__LINE__) << " distanceRules error =" << m_Model->lastError().text();
}

void DistanceRulesWidget::on_distanceRulesComboBox_currentIndexChanged(int index)
{
    //saveModel();
    m_Mapper->setCurrentIndex(distanceRulesComboBox->currentIndex());
}

void DistanceRulesWidget::on_addButton_clicked()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        Utils::Log::addError(this, "Unable to add row", __FILE__, __LINE__);
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    distanceRulesComboBox->setCurrentIndex(m_Model->rowCount()-1);
    distanceRulesUidLabel->setText(calcDistanceRulesUid());
    distanceRulesUidLabel->setFocus();
    //qDebug() << __FILE__ << QString::number(__LINE__) << " percentUidLabel =" << percentUidLabel->text();
    //qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());

}

void DistanceRulesWidget::on_deleteButton_clicked()
{
    if (!m_Model->removeRow(distanceRulesComboBox->currentIndex()))
    {
    	  Utils::Log::addError(this, "Unable to remove row", __FILE__, __LINE__);
        }
    distanceRulesComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void DistanceRulesWidget::saveToSettings(Core::ISettings *sets)
{
    if (!m_Model->submit()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("distancerules")));
        Utils::warningMessageBox(tr("Can not submit distancerules to your personnal database."),
                                 tr("An error occured during distancerules saving. Datas are corrupted."));
    }
        connect(typeEdit,SIGNAL(textEdited(const QString &)),distanceRulesComboBox,SLOT(setEditText(const QString &)));
        update();
}

void DistanceRulesWidget::writeDefaultSettings(Core::ISettings *s)
{
//    Utils::Log::addMessage("DistanceRulesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DistanceRulesWidget"));
//    s->sync();
}

void DistanceRulesWidget::changeEvent(QEvent *e)
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

QString DistanceRulesWidget::calcDistanceRulesUid(){
    QString uuidStr;
    uuidStr = QUuid::createUuid().toString();
    return uuidStr;
}

