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
#include "percentagespage.h"
#include <accountplugin/constants.h>

#include <accountbaseplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QRegExp>
#include <QLocale>

enum { WarnDebugMessage = false };

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


PercentagesPage::PercentagesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("PercentagesPage");
}

PercentagesPage::~PercentagesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PercentagesPage::id() const { return objectName(); }
QString PercentagesPage::displayName() const { return tkTr(Trans::Constants::PERCENT); }
QString PercentagesPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int PercentagesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 70;
}

void PercentagesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void PercentagesPage::apply()
{
    if (WarnDebugMessage)
        LOG("applyChanges");
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PercentagesPage::finish() { delete m_Widget; }

void PercentagesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *PercentagesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PercentagesWidget(parent);
    return m_Widget;
}

PercentagesWidget::PercentagesWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setObjectName("PercentagesWidget");
    setupUi(this);
    m_user_uid = user()->uuid();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " m_user_uid =" << m_user_uid ;
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
//    addButton->setText("New");
    deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
//    deleteButton->setText("Delete");

    m_Model = new AccountDB::PercentModel(this);
    // TODO: m_Model->setUserUuid();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " m_user_uid =" << m_user_uid ;
    userEditedLabel->setText(m_user_uid);
    percentUidLabel->setText("");
    //percentUidLabel->setFocus();
    //userEditedLabel->hide();
    //percentUidLabel->hide();
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(percentUidLabel,AccountDB::Constants::PERCENT_UID);
    m_Mapper->addMapping(userEditedLabel,AccountDB::Constants::PERCENT_USER_UID);//
    m_Mapper->addMapping(typeEdit, AccountDB::Constants::PERCENT_TYPE);
    m_Mapper->addMapping(valueDoubleSpinBox, AccountDB::Constants::PERCENT_VALUES);
    //m_Mapper->toFirst();
    percentagesComboBox->setModel(m_Model);
    percentagesComboBox->setModelColumn(AccountDB::Constants::PERCENT_TYPE);
    setDataToUi();
}

PercentagesWidget::~PercentagesWidget()
{
    //saveModel();
}

void PercentagesWidget::setDataToUi()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(percentagesComboBox->currentIndex());
    m_Mapper->setCurrentIndex(percentagesComboBox->currentIndex());
}

void PercentagesWidget::saveModel()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes?"),
                                          tr("You make changes into the percentages table.\n"
                                             "Do you want to save them?"));
        if (yes) {
           if (!m_Model->submit()) {
                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("percentages")));
            }
        }
        else {
            m_Model->revert();
        }
    }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " percentage error =" << m_Model->lastError().text();
}

void PercentagesWidget::on_percentagesComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    //saveModel();
    m_Mapper->setCurrentIndex(percentagesComboBox->currentIndex());
}

void PercentagesWidget::on_addButton_clicked()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        LOG_ERROR("Unable to add row");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    percentagesComboBox->setCurrentIndex(m_Model->rowCount()-1);
    userEditedLabel->setText(m_user_uid);
    userEditedLabel->setFocus();
    percentUidLabel->setText(calcPercentagesUid());
    percentUidLabel->setFocus();

    if (WarnDebugMessage) {
              qDebug() << __FILE__ << QString::number(__LINE__) << " userEditedLabel =" << userEditedLabel->text() ;
              qDebug() << __FILE__ << QString::number(__LINE__) << " percentUidLabel =" << percentUidLabel->text();
              qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    }
}

void PercentagesWidget::on_deleteButton_clicked()
{
    if (!m_Model->removeRow(percentagesComboBox->currentIndex())) {
          LOG_ERROR("Unable to remove row");
    }
    percentagesComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void PercentagesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    if (!m_Model->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("percentages")));
        Utils::warningMessageBox(tr("Can not submit percentages to your personnal database."),
                                 tr("An error occured during percentages saving. Data are corrupted."));
    }
    connect(typeEdit,SIGNAL(textEdited(const QString &)),percentagesComboBox,SLOT(setEditText(const QString &)));
    update();
}

void PercentagesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Utils::Log::addMessage("PercentagesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("PercentagesWidget"));
//    s->sync();
}

void PercentagesWidget::changeEvent(QEvent *e)
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

QString PercentagesWidget::calcPercentagesUid(){
    QString Uid = Utils::Database::createUid();
    /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << m_Model->rowCount() ;
    QModelIndex index = m_Model->index(m_Model->rowCount()-2,AccountDB::Constants::PERCENT_UID);
    if (!index.isValid())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid" ;
        }
    int percentageUidBefore = m_Model->data(index,Qt::DisplayRole).toInt();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " percentageUidBefore =" << QString::number(percentageUidBefore) ;
    int percentageUid =  percentageUidBefore + 1;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " percentageUid =" << QString::number(percentageUid);*/
    return Uid;
}

