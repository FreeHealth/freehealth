/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *       Pierre-Marie Desombre <MAIL@ADRESS>                               *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "availablemovementpage.h"
#include <accountplugin/constants.h>

#include <accountbaseplugin/availablemovementmodel.h>
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
AvailableMovementPage::AvailableMovementPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("AvailableMovementPage"); }

AvailableMovementPage::~AvailableMovementPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString AvailableMovementPage::id() const { return objectName(); }
QString AvailableMovementPage::name() const { return tkTr(Trans::Constants::AVAILABLE_MOVEMENTS); }
QString AvailableMovementPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void AvailableMovementPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void AvailableMovementPage::applyChanges()
{qDebug() << __FILE__ << QString::number(__LINE__) << " applyChanges ";
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AvailableMovementPage::finish() { delete m_Widget; }

void AvailableMovementPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    QStandardItemModel * model = availableMovementModelByLocale();
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *AvailableMovementPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AvailableMovementWidget(parent);
    return m_Widget;
}

AvailableMovementWidget::AvailableMovementWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setupUi(this);
    taxDeductibilityComboBox->addItem(tkTr(Trans::Constants::NO));//0
    taxDeductibilityComboBox->addItem(tkTr(Trans::Constants::YES));//1
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    removeButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    QString add = trUtf8("Add");
    QString less = trUtf8("Less");
    typeComboBox->addItem(theme()->icon(Core::Constants::ICONADD),add);
    typeComboBox->addItem(theme()->icon(Core::Constants::ICONADD),less);
    QString null;
    parentComboBox->setEditable(true);
    parentComboBox->addItem(null);
    m_Model = new AccountDB::AvailableMovementModel(this);
    /** \todo  m_Model->setUserUuid(); */
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(currentLabel, AccountDB::Constants::AVAILMOV_LABEL,"text");
    m_Mapper->addMapping(typeComboBox, AccountDB::Constants::AVAILMOV_TYPE,"currentText");
    m_Mapper->addMapping(commentEdit, AccountDB::Constants::AVAILMOV_COMMENT,"text");
    m_Mapper->addMapping(parentComboBox, AccountDB::Constants::AVAILMOV_PARENT,"currentText");//parent movement
    m_Mapper->addMapping(taxDeductibilityComboBox,AccountDB::Constants::AVAILMOV_DEDUCTIBILITY,"currentIndex");
    movComboBox->setModel(m_Model);
    movComboBox->setModelColumn(AccountDB::Constants::AVAILMOV_LABEL);
    setDatasToUi();
}

AvailableMovementWidget::~AvailableMovementWidget()
{
    saveModel();
}

void AvailableMovementWidget::setDatasToUi()
{
    m_Mapper->setCurrentIndex(movComboBox->currentIndex());
}


void AvailableMovementWidget::saveModel()
{
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes ?"),
                                          tr("You make changes into available movements.\n"
                                             "Do you want to save them ?"));
        if (yes) {
            if (!m_Model->submit()) {
                qWarning() << __FILE__ << QString::number(__LINE__) << "model error = "<< m_Model->lastError().text() ;
                Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("available_movement")));
            }
        } else {
            m_Model->revert();
        }
    }
}

void AvailableMovementWidget::on_movComboBox_currentIndexChanged(int index)
{
    //saveModel();
    m_Mapper->setCurrentIndex(movComboBox->currentIndex());
}

void AvailableMovementWidget::on_addButton_clicked()
{
    if (!m_Model->insertRow(m_Model->rowCount()))
        Utils::Log::addError(this, "Unable to add row", __FILE__, __LINE__);
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_Model->rowCount()) ;
    movComboBox->setCurrentIndex(m_Model->rowCount() - 1);
    taxDeductibilityComboBox->setFocus();
    taxDeductibilityComboBox->setCurrentIndex(1);
    typeComboBox->setFocus();
}

void AvailableMovementWidget::on_removeButton_clicked()
{
    m_Model->removeRow(movComboBox->currentIndex());
    movComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}


void AvailableMovementWidget::saveToSettings(Core::ISettings *sets)
{
    if (!m_Model->submit()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("available_movement")));
        Utils::warningMessageBox(tr("Can not submit available movements to your personnal database."),
                                 tr("An error occured during available movements saving. Datas are corrupted."));
    }
}

void AvailableMovementWidget::writeDefaultSettings(Core::ISettings *s)
{
//    Utils::Log::addMessage("AvailableMovementWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("AvailableMovementWidget"));
//    s->sync();
}

void AvailableMovementWidget::changeEvent(QEvent *e)
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

QStandardItemModel * AvailableMovementPage::availableMovementModelByLocale(){
    QStandardItemModel * model = new QStandardItemModel;
    QLocale locale;
    QString localCountry;
    localCountry = QLocale::countryToString(locale.country());
    localCountry.remove("\"");
    qDebug() << __FILE__ << QString::number(__LINE__) << " locale Country =" << localCountry ;
    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
    QDir dirFreeAccountSql(sqlDirPath);
    QStringList filters ;
    filters << ".csv";
    QStringList listOfFiles;
    listOfFiles = dirFreeAccountSql.entryList(filters);
    QString strNamesOfFiles;
    foreach(strNamesOfFiles,listOfFiles){
        if (strNamesOfFiles.contains(localCountry))
        {
        	  qWarning() << __FILE__ << QString::number(__LINE__) << "No available movements for " << localCountry ;
            }
        else
        {
        	QFile file(sqlDirPath+"/"+strNamesOfFiles);
        	if (!file.open(QIODevice::ReadOnly))
        	{
        		  qWarning() << __FILE__ << QString::number(__LINE__) << strNamesOfFiles+" cannot open" ;
        	    }
        	QTextStream stream(&file);
        	while (!stream.atEnd())
        	{
        		int row = 0;
        		QString line = stream.readLine();
        		line.remove("\"");
        		line.remove("'");
        		QStringList listOfSeparators;
        		listOfSeparators << "," << ";" << QString("\t");
        		QString separator;
        		QString separatorStr;
        		foreach(separatorStr,listOfSeparators){
        		    if (line.contains(separatorStr))
        		    {
        		    	  separator = separatorStr;
        		        }
        		    }
        		if (!line.contains("AVAILMOV_ID"))
        		{
        			  //"AVAILMOV_ID","PARENT","TYPE","LABEL","CODE","COMMENT"
        			  QList<QStandardItem*> listOfItemsData;
        			  QStringList listOfItems;
        			  listOfItems = line.split(separator);
        			  for (int i = 0; i < AccountDB::Constants::AVAILMOV_MaxParam; i += 1)
        			  {
        			  	//model->setData(model->index(row,i),listOfItems[i],Qt::EditRole);
        			  	QStandardItem * item = new QStandardItem;
        			  	item->setData(listOfItems[i]);
        			  	listOfItemsData << item;
        			  }
        			model->appendRow(listOfItemsData);
        			row++;  
        		    }
        	    }
            }
        }
    return model;
}
