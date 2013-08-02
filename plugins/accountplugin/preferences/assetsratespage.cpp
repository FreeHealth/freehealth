/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "assetsratespage.h"
#include <accountplugin/constants.h>

#include <accountbaseplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <translationutils/constants.h>
#include <translationutils/trans_account.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>

#include <coreplugin/constants_icons.h>

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QRegExp>
#include <QLocale>

enum { WarnDebugMessage = false };

using namespace Account;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  AccountUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
AssetsRatesPage::AssetsRatesPage(QObject *parent) :
    IOptionsPage(parent), m_Widget(0)
{
    setObjectName("AssetsRatesPage");
}

AssetsRatesPage::~AssetsRatesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString AssetsRatesPage::id() const { return objectName(); }
QString AssetsRatesPage::displayName() const { return tkTr(Trans::Constants::ASSETSRATES); }
QString AssetsRatesPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int AssetsRatesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 10;
}

void AssetsRatesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void AssetsRatesPage::apply()
{
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " applyChanges ";
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AssetsRatesPage::finish() {
    delete m_Widget;
}

void AssetsRatesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *AssetsRatesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AssetsRatesWidget(parent);
    return m_Widget;
}

AssetsRatesWidget::AssetsRatesWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    //QCoreApplication::processEvents(QEventLoop::AllEvents);
    setObjectName("AssetsRatesWidget");
    setupUi(this);
    dateEdit->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));

    m_user_uid = user()->uuid();
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    beginSpinBox->setRange(0, 1000);
    endSpinBox->setRange(0, 1000);
    rateDoubleSpinBox->setRange(0.00, 1000.00);
    rateDoubleSpinBox->setSingleStep(0.01);
    m_Model = new AccountDB::AssetsRatesModel(this);
    assetsRatesUidLabel->setText("");

    nameEdit->setPlaceholderText(tr("Please enter a name here"));
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(assetsRatesUidLabel,AccountDB::Constants::ASSETSRATES_USER_UID);
    m_Mapper->addMapping(nameEdit, AccountDB::Constants::ASSETSRATES_NAME);
    m_Mapper->addMapping(dateEdit, AccountDB::Constants::ASSETSRATES_DATE);
    m_Mapper->addMapping(rateDoubleSpinBox, AccountDB::Constants::ASSETSRATES_RATES);
    //m_Mapper->toFirst();
    assetsNameComboBox->setModel(m_Model);
    assetsNameComboBox->setModelColumn(AccountDB::Constants::ASSETSRATES_NAME);
    setDataToUi();
    connect(m_Mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSpinBoxes(int)));
//    connect(createDefaults, SIGNAL(clicked()), this, SLOT(createDefaultAssetsRates()));
}

AssetsRatesWidget::~AssetsRatesWidget()
{
    //saveModel();
}

void AssetsRatesWidget::setDataToUi()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(assetsNameComboBox->currentIndex());
    m_Mapper->setCurrentIndex(assetsNameComboBox->currentIndex());
}

void AssetsRatesWidget::saveModel()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes?"),
                                          tr("You make changes into the assetsrates table.\n"
                                             "Do you want to save them?"));
        if (yes) {
           if (!m_Model->submit()) {if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " assetsrates no submit ";
                LOG_ERROR( tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("assetsrates")));
            }
        } else {
            m_Model->revert();
        }
    }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " assets rates error =" << m_Model->lastError().text();
}

void AssetsRatesWidget::on_assetsNameComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    m_Mapper->setCurrentIndex(assetsNameComboBox->currentIndex());
}

void AssetsRatesWidget::on_addButton_clicked()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        LOG_ERROR( "Unable to add row");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    assetsNameComboBox->setCurrentIndex(m_Model->rowCount()-1);
    assetsRatesUidLabel->setText(m_user_uid);
    assetsRatesUidLabel->setFocus();
    QDate currentDate = QDate::currentDate();
    dateEdit->setDate(currentDate);
    dateEdit->setFocus();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());

}

void AssetsRatesWidget::on_deleteButton_clicked()
{
    if (!m_Model->removeRow(assetsNameComboBox->currentIndex())) {
        LOG_ERROR("Unable to remove row");
    }
    assetsNameComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}


void AssetsRatesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    if (!m_Model->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("assetsrates")));
        Utils::warningMessageBox(tr("Can not submit assetsrates to your personnal database."),
                                 tr("An error occured during assetsrates saving. Data are corrupted."));
    }
    if (!insertYearsRange()) {
        LOG_ERROR("Unable to insert years range");
    }
    connect(nameEdit,SIGNAL(textEdited(const QString &)),assetsNameComboBox,SLOT(setEditText(const QString &)));
    update();
}

void AssetsRatesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Utils::Log::addMessage("AssetsRatesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("AssetsRatesWidget"));
//    s->sync();
}

void AssetsRatesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        addButton->setToolTip(tkTr(Trans::Constants::FILENEW_TEXT));
        deleteButton->setToolTip(tkTr(Trans::Constants::REMOVE_TEXT));
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

QString AssetsRatesWidget::calcAssetsRatesUid()
{
    QString uuidStr;
    uuidStr = Utils::Database::createUid();
    return uuidStr;
}

bool AssetsRatesWidget::insertYearsRange()
{
    bool ret = true;
    QString beginYear = QString::number(beginSpinBox->value());
    QString endYear = QString::number(endSpinBox->value());
    QString yearRange = QString("%1_%2").arg(beginYear, endYear);
    m_Model->setFilter("");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " m_Model->rowCount =" << QString::number((m_Model->rowCount())) ;

    if (!m_Model->setData(m_Model->index(m_Model->rowCount()-1, AccountDB::Constants::ASSETSRATES_YEARS),
                          yearRange, Qt::EditRole)) {
        LOG_ERROR("unable to insert years range: " + m_Model->lastError().text());
    }
    return ret;
}

//static QString getCsvDefaultFile()
//{
//    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
//    QDir dir(sqlDirPath);
//    if (!dir.exists())
//        return QString();
//    QString fileName = QString("assets_rates_%1.csv").arg(QLocale().name());
//    QFile file(dir.absolutePath() + QDir::separator() + fileName);
//    if (!file.exists())
//        return QString();
//    return file.fileName();
//}

//void AssetsRatesWidget::createDefaultAssetsRates()
//{
//    writeDefaultsWithLocale();
//}

//QStandardItemModel *AssetsRatesWidget::assetsRatesModelByLocale()
//            }
//    }
//    return model;
//}

//bool AssetsRatesWidget::writeDefaultsWithLocale()
//{
//    bool test = false;
//    QStandardItemModel * model = assetsRatesModelByLocale();
//    int assetsratesModelRows = model->rowCount();
//    QString strList;
//    for (int i = 0; i < assetsratesModelRows; i += 1) {
//        if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex())) {
//    	    qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(m_Model->rowCount()) ;
//        }
//        QString strValues;
//        for (int j = 0; j < AccountDB::Constants::ASSETSRATES_MaxParam ; j += 1) {
//            QStandardItem * item = model->itemFromIndex(model->index(i,j));
//            QVariant value = item->data();
//            if (value.canConvert(QVariant::String)) {
//                QString strValue = value.toString().replace("'","''");
//                value = QVariant::fromValue(strValue);
//            }
//            strValues += value.toString()+" ";
//            if (!m_Model->setData(m_Model->index(m_Model->rowCount()-1,j),value,Qt::EditRole)) {
//                qWarning() << __FILE__ << QString::number(__LINE__) << "data not inserted !" ;
//            }
//        }
//        strList += strValues+"\n";
//        test = m_Model->submit();
//    }
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " values =\n" << strList;

//    return test;
//}

void AssetsRatesWidget::updateSpinBoxes(int index)
{
    Q_UNUSED(index);
    int beginYear = 0;
    int endYear = 0;
    AccountDB::AssetsRatesModel model(this);
    QString range = model.data(model.index(index,AccountDB::Constants::ASSETSRATES_NAME),Qt::DisplayRole).toString();
    QString filter = QString("%1='%2' AND %3='%4'").arg("USER_UID",m_user_uid,"NAME",range);
    model.setFilter(filter);
    QString years = model.data(m_Model->index(0,AccountDB::Constants::ASSETSRATES_YEARS),Qt::DisplayRole).toString();
    QStringList listYears = years.split("_");
    if (listYears.size()>1) {
        beginYear = listYears[0].toInt();
        endYear= listYears[1].toInt();
        }
    beginSpinBox->setValue(beginYear);
    endSpinBox->setValue(endYear);
}
