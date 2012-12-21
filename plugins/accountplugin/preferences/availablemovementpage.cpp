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
#include <coreplugin/constants_menus.h>

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCompleter>

enum { WarnDebugMessage = false };

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }


AvailableMovementPage::AvailableMovementPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("AvailableMovementPage");
}

AvailableMovementPage::~AvailableMovementPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString AvailableMovementPage::id() const { return objectName(); }
QString AvailableMovementPage::displayName() const { return tkTr(Trans::Constants::AVAILABLE_MOVEMENTS); }
QString AvailableMovementPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int AvailableMovementPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 30;
}

void AvailableMovementPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void AvailableMovementPage::apply()
{
    if (WarnDebugMessage)
        LOG("applyChanges");
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
    //QCoreApplication::processEvents(QEventLoop::AllEvents);
    setupUi(this);
    taxDeductibilityComboBox->addItem(tkTr(Trans::Constants::NO));//0
    taxDeductibilityComboBox->addItem(tkTr(Trans::Constants::YES));//1
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    removeButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    QString add = tr("Add");
    QString less = tr("Less");
    typeComboBox->addItem(theme()->icon(Core::Constants::ICONADD),less);
    typeComboBox->addItem(theme()->icon(Core::Constants::ICONADD),add);
    m_completionList << tr("Receipts");
}

AvailableMovementWidget::~AvailableMovementWidget()
{
    //saveModel();
}

void AvailableMovementWidget::setDataToUi()
{
    m_Mapper->setCurrentIndex(movComboBox->currentIndex());
}

void AvailableMovementWidget::fillParentCombo(){
}

void AvailableMovementWidget::fillHugeWidgets(){
     m_Model = new AccountDB::AvailableMovementModel(this);
    // TODO:  m_Model->setUserUuid();
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(currentLabel, AccountDB::Constants::AVAILMOV_LABEL);
    m_Mapper->addMapping(typeComboBox, AccountDB::Constants::AVAILMOV_TYPE,"currentIndex");
    m_Mapper->addMapping(commentEdit, AccountDB::Constants::AVAILMOV_COMMENT);
    m_Mapper->addMapping(codeEdit, AccountDB::Constants::AVAILMOV_CODE,"text");
    m_Mapper->addMapping(parentEdit, AccountDB::Constants::AVAILMOV_PARENT,"text");//parent movement
    m_Mapper->addMapping(taxDeductibilityComboBox,AccountDB::Constants::AVAILMOV_DEDUCTIBILITY,"currentIndex");
    movComboBox->setModel(m_Model);
    movComboBox->setModelColumn(AccountDB::Constants::AVAILMOV_LABEL);
    setDataToUi();
}

void AvailableMovementWidget::saveModel()
{
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes?"),
                                          tr("You make changes into available movements.\n"
                                             "Do you want to save them?"));
        if (yes) {
            if (!m_Model->submit()) {
                qWarning() << __FILE__ << QString::number(__LINE__) << "model error = "<< m_Model->lastError().text() ;
                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("available_movement")));
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
    QString parentText = m_Model->data(m_Model->index(index,AccountDB::Constants::AVAILMOV_PARENT),Qt::DisplayRole).toString();
    parentEdit->setText(parentText);
    m_completionList << parentText;
    QString codeText = m_Model->data(m_Model->index(index,AccountDB::Constants::AVAILMOV_CODE),Qt::DisplayRole).toString();
    codeEdit->setText(codeText);
}

void AvailableMovementWidget::on_addButton_clicked()
{
    if (!m_Model->insertRow(m_Model->rowCount()))
        LOG_ERROR("Unable to add row");
    if (WarnDebugMessage)
        LOG("rowCount =" + QString::number(m_Model->rowCount()));
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
    Q_UNUSED(sets);
    if (!m_Model)
        return;
    if (!m_Model->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("available_movement")));
        Utils::warningMessageBox(tr("Can not submit available movements to your personnal database."),
                                 tr("An error occured during available movements saving. Data are corrupted."));
    }
}

void AvailableMovementWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
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

void AvailableMovementWidget::showEvent(QShowEvent *event){
    Q_UNUSED(event);
    fillHugeWidgets();
}

//static QString getCsvDefaultFile()
//{
//    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
//    QDir dir(sqlDirPath);
//    if (!dir.exists())
//        return QString();
//    QString fileName = QString("available_movements_%1.csv").arg(QLocale().name());
//    QFile file(dir.absolutePath() + QDir::separator() + fileName);
//    if (!file.exists())
//        return QString();
//    return file.fileName();
//}

//QStandardItemModel *AvailableMovementWidget::availableMovementModelByLocale()
//{
//    QStandardItemModel *model = new QStandardItemModel;
//    QString csvFileName = getCsvDefaultFile();
//    QFile file(getCsvDefaultFile());
//    // some validity checking
//    if (!file.exists()) {
//        LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(QLocale().name() + " " + tr("Available Movements")));
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
//        foreach(separatorStr,listOfSeparators){
//            if (line.contains(separatorStr)){
//                separator = separatorStr;
//                }
//            }
//        if (!line.contains("AVAILMOV_ID")){
//            //"AVAILMOV_ID","PARENT","TYPE","LABEL","CODE","COMMENT","DEDUCTIBILITY"
//            QList<QStandardItem*> listOfItemsData;
//            QStringList listOfItems;
//            listOfItems = line.split(separator);
//            for (int i = 0; i < AccountDB::Constants::AVAILMOV_MaxParam ; i += 1){
//                //model->setData(model->index(row,i),listOfItems[i],Qt::EditRole);
//        	QStandardItem * item = new QStandardItem;
//        	QString itemOfList = listOfItems[i];
//        	itemOfList.remove("\"");
//        	item->setData(itemOfList);
//        	listOfItemsData << item;
//        	}
//            model->appendRow(listOfItemsData);
//            ++row;
//            }
//    }
//    return model;
//}

//bool AvailableMovementWidget::fillEmptyAvailableModel()
//{
//    bool test = false;
//    QStandardItemModel * model = availableMovementModelByLocale();
//    int availModelRows = model->rowCount();
//    QString strList;
//    for (int i = 0; i < availModelRows; i += 1){
//        if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex()))
//    	  		{qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(m_Model->rowCount()) ;
//    	  		    }
//    	  		    QString strValues;
//    	  	for (int j = 0; j < AccountDB::Constants::AVAILMOV_MaxParam ; j += 1){
//    	  		QStandardItem * item = model->itemFromIndex(model->index(i,j));
//    	  		QVariant value = item->data();
//    	  		//todo, semantics
//    	  		if (value.canConvert(QVariant::String))
//    	  		{
//    	  			  QString strValue = value.toString().replace("'","''");
//    	  			  value = QVariant::fromValue(strValue);
//    	  		    }
//    	  		    strValues += value.toString()+" ";
//    	  		if (!m_Model->setData(m_Model->index(m_Model->rowCount()-1,j),value,Qt::EditRole))
//    	  		{
//    	  			qWarning() << __FILE__ << QString::number(__LINE__) << "data not inserted !" ;
//    	  		    }
//    	  	    }
//    	  	    strList += strValues+"\n";
//    	      test = m_Model->submit();
//    	      }
//
//    return test;
//}

void AvailableMovementWidget::setCompletionList(const QString & text){
    m_completionList << text;
    m_completionList.removeDuplicates();
}

void AvailableMovementWidget::on_parentEdit_textChanged(const QString & text){
    Q_UNUSED(text);
    QCompleter *c = new QCompleter(m_completionList,this);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    parentEdit->setCompleter(c);
}
