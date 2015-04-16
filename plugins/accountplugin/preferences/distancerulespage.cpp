/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "distancerulespage.h"
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
#include <QDir>
#include <QFile>

enum { WarnDebugMessage = false };

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


DistanceRulesPage::DistanceRulesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("DistanceRulesPage");
}

DistanceRulesPage::~DistanceRulesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DistanceRulesPage::id() const { return objectName(); }
QString DistanceRulesPage::displayName() const { return tkTr(Trans::Constants::DISTRULES); }
QString DistanceRulesPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int DistanceRulesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 50;
}

void DistanceRulesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void DistanceRulesPage::apply()
{
    if (WarnDebugMessage)
        LOG("applyChanges");
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
    deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    preferredSpinBox->setRange(0,1);

    m_Model = new AccountDB::DistanceRulesModel(this);
    // TODO:  m_Model->setUserUuid();
    distanceRulesUidLabel->setText("");
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(distanceRulesUidLabel,AccountDB::Constants::DISTRULES_UID);
    m_Mapper->addMapping(typeEdit, AccountDB::Constants::DISTRULES_TYPE);
    m_Mapper->addMapping(valueDoubleSpinBox, AccountDB::Constants::DISTRULES_VALUES);
    m_Mapper->addMapping(preferredSpinBox, AccountDB::Constants::DISTRULES_PREF);
    m_Mapper->addMapping(minKmDoubleSpinBox, AccountDB::Constants::DISTRULES_MIN_KM);
    //m_Mapper->toFirst();
    distanceRulesComboBox->setModel(m_Model);
    distanceRulesComboBox->setModelColumn(AccountDB::Constants::DISTRULES_TYPE);
    setDataToUi();
}

DistanceRulesWidget::~DistanceRulesWidget()
{
    //saveModel();
}

void DistanceRulesWidget::setDataToUi()
{
    if (WarnDebugMessage)
        LOG("index row  =" + QString::number(distanceRulesComboBox->currentIndex()));
    m_Mapper->setCurrentIndex(distanceRulesComboBox->currentIndex());
}

void DistanceRulesWidget::saveModel()
{
    if (WarnDebugMessage)
        LOG("currentIndex =" + QString::number(m_Mapper->currentIndex()));
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes?"),
                                          tr("You make changes into the distancerules table.\n"
                                             "Do you want to save them?"));
        if (yes) {
           if (!m_Model->submit()) {if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " distancerules no submit ";
                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("distancerules")));
            }
        }
        else {
            m_Model->revert();
        }
    }
    if (WarnDebugMessage)
        LOG("distanceRules error =" + m_Model->lastError().text());
}

void DistanceRulesWidget::on_distanceRulesComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    //saveModel();
    m_Mapper->setCurrentIndex(distanceRulesComboBox->currentIndex());
}

void DistanceRulesWidget::on_addButton_clicked()
{
    if (WarnDebugMessage)
        LOG("rowCount1 =" + QString::number(m_Model->rowCount()));
    if (!m_Model->insertRow(m_Model->rowCount()))
        LOG_ERROR("Unable to add row");
    if (WarnDebugMessage)
        LOG("rowCount2 =" + QString::number(m_Model->rowCount()));
    distanceRulesComboBox->setCurrentIndex(m_Model->rowCount()-1);
    distanceRulesUidLabel->setText(calcDistanceRulesUid());
    distanceRulesUidLabel->setFocus();
    typeEdit->setFocus();
}

void DistanceRulesWidget::on_deleteButton_clicked()
{
    if (!m_Model->removeRow(distanceRulesComboBox->currentIndex())) {
        LOG_ERROR("Unable to remove row");
    }
    distanceRulesComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void DistanceRulesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    if (!m_Model->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("distancerules")));
        Utils::warningMessageBox(tr("Can not submit distancerules to your personnal database."),
                                 tr("An error occured during distancerules saving. Data are corrupted."));
    }
    connect(typeEdit,SIGNAL(textEdited(const QString &)),distanceRulesComboBox,SLOT(setEditText(const QString &)));
    update();
}

void DistanceRulesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Utils::Log::addMessage("DistanceRulesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DistanceRulesWidget"));
//    s->sync();
}

void DistanceRulesWidget::changeEvent(QEvent *e)
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

QString DistanceRulesWidget::calcDistanceRulesUid(){
    QString uuidStr;
    uuidStr = Utils::Database::createUid();
    return uuidStr;
}

//static QString getCsvDefaultFile()
//{
//    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
//    QDir dir(sqlDirPath);
//    if (!dir.exists())
//        return QString();
//    QString fileName = QString("distance_rules_%1.csv").arg(QLocale().name());
//    QFile file(dir.absolutePath() + QDir::separator() + fileName);
//    if (!file.exists())
//        return QString();
//    return file.fileName();
//}

//QStandardItemModel *DistanceRulesWidget::distanceRulesModelByLocale()
//{
//    QStandardItemModel *model = new QStandardItemModel;
//    QString csvFileName = getCsvDefaultFile();
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " csvFileName =" << csvFileName ;
//    QFile file(getCsvDefaultFile());
//    // some validity checking
//    if (!file.exists()) {
//        LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(QLocale().name() + " " + tr("Distance rules")));
//        return model;
//    }
//    if (!file.open(QFile::ReadOnly | QFile::Text)) {
//        LOG_ERROR(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file.fileName()));
//        return model;
//    }

//    // read the content with UTF8 coding system
//    QTextStream stream(&file);
//    stream.setCodec("UTF-8");
//    // skip first line
//    //stream.readLine();
//    //int row = 0;
//    while (!stream.atEnd())
//    {
//        int row = 0;
//        QString line = stream.readLine();
//        QStringList listOfSeparators;
//        listOfSeparators << ",\"" << ";\"" << QString("\t\"")
//                         << ",''" << ";''" << QString("\t''");
//        QString separator;
//        QString separatorStr;
//        foreach(separatorStr,listOfSeparators) {
//            if (line.contains(separatorStr)) {
//                separator = separatorStr;
//            }
//        }
//        if (!line.contains("DISTRULES_ID")){
//            //"AVAILMOV_ID","PARENT","TYPE","LABEL","CODE","COMMENT","DEDUCTIBILITY"
//            QList<QStandardItem*> listOfItemsData;
//            QStringList listOfItems;
//            listOfItems = line.split(separator);
//            for (int i = 0; i < AccountDB::Constants::DISTRULES_MaxParam ; i += 1){
//                //model->setData(model->index(row,i),listOfItems[i],Qt::EditRole);
//        	QStandardItem * item = new QStandardItem;
//        	if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " listOfItems[i] =" << listOfItems[i] ;
//        	QString itemOfList = listOfItems[i];
//        	itemOfList.remove("\"");
//        	itemOfList.remove("'");
//        	item->setData(itemOfList);
//        	listOfItemsData << item;
//            }
//            model->appendRow(listOfItemsData);
//            ++row;
//        }
//    }
//    return model;
//}

//bool DistanceRulesWidget::fillEmptyDistanceRulesModel(){
//    bool test = false;
//    QStandardItemModel * model = distanceRulesModelByLocale();
//    int availModelRows = model->rowCount();
//    //if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " availModelRows = " << QString::number(availModelRows) ;
//    QString strList;
//    for (int i = 0; i < availModelRows; i += 1){
//        if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex()))
//    	  		{qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(m_Model->rowCount()) ;
//    	  		    }
//    	  		    QString strValues;
//    	  	for (int j = 0; j < AccountDB::Constants::DISTRULES_MaxParam ; j += 1){
//    	  		QStandardItem * item = model->itemFromIndex(model->index(i,j));
//    	  		QVariant value = item->data();
//    	  		//todo, semantics
//    	  		if (value.canConvert(QVariant::String))
//    	  		{
//    	  			  QString strValue = value.toString().replace("'","''");
//    	  			  value = QVariant::fromValue(strValue);
//    	  		    }
//    	  		    strValues += value.toString()+" ";
//    	  		//if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
//    	  		//if (WarnDebugMessage)
//   	      qDebug() << __FILE__ << QString::number(__LINE__) << "m_Model->rowCount() =" << QString::number(m_Model->rowCount()) ;
//    	  		if (!m_Model->setData(m_Model->index(m_Model->rowCount()-1,j),value,Qt::EditRole))
//    	  		{
//    	  			qWarning() << __FILE__ << QString::number(__LINE__) << "data not inserted !" ;
//    	  		    }
//    	  	    }
//    	  	    strList += strValues+"\n";
//    	      test = m_Model->submit();
//    	      }
//    	      if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " values =\n" << strList;

//    return test;
//}

