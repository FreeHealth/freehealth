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

#include <QDir>
#include <QFile>
#include <QTextStream>


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
    if (m_Model->rowCount() < 1)
    {
    	  if (!fillEmptyAvailableModel())
    	  {
    	  	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to fill availablemodel whith local .csv"),
    	  	                       QMessageBox::Ok);
    	      }
        }
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

static QString getCsvDefaultFile()
{
    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
    QDir dir(sqlDirPath);
    if (!dir.exists())
        return QString();
    QString fileName = QString("available_movements_%1.csv").arg(QLocale().name());
    QFile file(dir.absolutePath() + QDir::separator() + fileName);
    if (!file.exists())
        return QString();
    return file.fileName();
}

QStandardItemModel *AvailableMovementWidget::availableMovementModelByLocale()
{
    QStandardItemModel *model = new QStandardItemModel;
    QString csvFileName = getCsvDefaultFile();
    qDebug() << __FILE__ << QString::number(__LINE__) << " csvFileName =" << csvFileName ;
    QFile file(getCsvDefaultFile());
    // some validity checking
    if (!file.exists()) {
        LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(QLocale().name() + " " + tr("Available Movements")));
        return model;
    }
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file.fileName()));
        return model;
    }

    // read the content with UTF8 coding system
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    // skip first line
    //stream.readLine();
    int row = 0;
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
            if (line.contains(separatorStr)){
                separator = separatorStr;
                }
            }
        if (!line.contains("AVAILMOV_ID")){
            //"AVAILMOV_ID","PARENT","TYPE","LABEL","CODE","COMMENT","DEDUCTIBILITY"
            QList<QStandardItem*> listOfItemsData;
            QStringList listOfItems;
            listOfItems = line.split(separator);
            for (int i = 0; i < AccountDB::Constants::AVAILMOV_MaxParam ; i += 1){
                //model->setData(model->index(row,i),listOfItems[i],Qt::EditRole);
        	QStandardItem * item = new QStandardItem;
        	//qDebug() << __FILE__ << QString::number(__LINE__) << " listOfItems[i] =" << listOfItems[i] ;
        	item->setData(listOfItems[i]);
        	listOfItemsData << item;
        	}
            model->appendRow(listOfItemsData);
            ++row;  
            }
    }
    return model;
}


bool AvailableMovementWidget::fillEmptyAvailableModel(){
    bool test = false;
    QStandardItemModel * model = availableMovementModelByLocale();
    int availModelRows = model->rowCount();
    qDebug() << __FILE__ << QString::number(__LINE__) << " availModelRows = " << QString::number(availModelRows) ;
    for (int i = 0; i < availModelRows; i += 1){
        if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex()))
    	  		{qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(m_Model->rowCount()) ;
    	  			  /*QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to insert row \n")
    	  			  +__FILE__+QString::number(__LINE__),QMessageBox::Ok);*/
    	  		    }
    	  	for (int j = 0; j < AccountDB::Constants::AVAILMOV_MaxParam ; j += 1){
    	  		QStandardItem * item = model->itemFromIndex(model->index(i,j));
    	  		QVariant value = item->data();
    	  		qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
    	  		qDebug() << __FILE__ << QString::number(__LINE__) << "m_Model->rowCount() =" << QString::number(m_Model->rowCount()) ;
    	  		if (!m_Model->setData(m_Model->index(m_Model->rowCount()-1,j),value,Qt::EditRole))
    	  		{
    	  			qWarning() << __FILE__ << QString::number(__LINE__) << "data not inserted !" ;  
    	  		    }
    	  	    }
    	      test = m_Model->submit();
    	      }
    return test;
}


