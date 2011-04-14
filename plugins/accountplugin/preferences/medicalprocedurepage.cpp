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
 *   Contributors : Pierre-Marie Desombre,<pm.desombre@medsyn.fr>          *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "medicalprocedurepage.h"
#include <accountplugin/constants.h>

#include <accountbaseplugin/medicalproceduremodel.h>
#include <accountbaseplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>

#include <coreplugin/constants_icons.h>

#include <QCompleter>
#include <QDir>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  AccountUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
MedicalProcedurePage::MedicalProcedurePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("MedicalProcedurePage"); }

MedicalProcedurePage::~MedicalProcedurePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString MedicalProcedurePage::id() const { return objectName(); }
QString MedicalProcedurePage::name() const { return tkTr(Trans::Constants::MEDICAL_PROCEDURES); }
QString MedicalProcedurePage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void MedicalProcedurePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void MedicalProcedurePage::applyChanges()
{qDebug() << __FILE__ << QString::number(__LINE__) << " applyChanges ";
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void MedicalProcedurePage::finish() { delete m_Widget; }

void MedicalProcedurePage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *MedicalProcedurePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new MedicalProcedureWidget(parent);
    return m_Widget;
}

MedicalProcedureWidget::MedicalProcedureWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setObjectName("MedicalProcedureWidget");
    setupUi(this);
    m_user_uid = user()->value(Core::IUser::Uuid).toString();
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }
    save->hide();
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    addButton->setText("New");
    removeButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    removeButton->setText("Delete");
    ownersComboBox->addItem(m_user_fullName,QVariant());
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    dateEdit->setDate(QDate::currentDate());
    rateSpin->setRange(0.00,100.00);
    m_Model = new AccountDB::MedicalProcedureModel(this);
    if (m_Model->rowCount() < 1)
    {
    	  if (!fillEmptyMPModel())
    	  {
    	  	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to fill medicalprocedure whith local .csv"),
    	  	                       QMessageBox::Ok);
    	      }
        }
        /** \todo  m_Model->setUserUuid(); */
    QLabel *mpUidLabel = new QLabel(this);
    mpUidLabel->setText("NULL");
    mpUidLabel->hide();
    userUidLabel->setText(m_user_uid);
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    //m_Mapper->addMapping(mpIDLabel, AccountDB::Constants::MP_ID, "ID");
    m_Mapper->addMapping(mpUidLabel, AccountDB::Constants::MP_UID);
    m_Mapper->addMapping(userUidLabel, AccountDB::Constants::MP_USER_UID);
    m_Mapper->addMapping(name, AccountDB::Constants::MP_NAME);
    //m_Mapper->addMapping(mpComboBox, AccountDB::Constants::MP_NAME);
    m_Mapper->addMapping(abstractEdit, AccountDB::Constants::MP_ABSTRACT);
    m_Mapper->addMapping(type, AccountDB::Constants::MP_TYPE);
    m_Mapper->addMapping(amountSpin, AccountDB::Constants::MP_AMOUNT);
    m_Mapper->addMapping(rateSpin, AccountDB::Constants::MP_REIMBOURSEMENT);
    m_Mapper->addMapping(dateEdit, AccountDB::Constants::MP_DATE);

    mpComboBox->setModel(m_Model);
    mpComboBox->setModelColumn(AccountDB::Constants::MP_NAME);
    setDatasToUi();
}

MedicalProcedureWidget::~MedicalProcedureWidget()
{
    //saveModel();
}

void MedicalProcedureWidget::setDatasToUi()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(mpComboBox->currentIndex());
    m_Mapper->setCurrentIndex(mpComboBox->currentIndex());
}

void MedicalProcedureWidget::saveModel()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    if (!m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes ?"),
                                          tr("You make changes into the medical procedures table.\n"
                                             "Do you want to save them ?"));
        if (yes) {
            if (!m_Model->submit()) {
                Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("medical_procedures")), __FILE__, __LINE__);
            }
        } else {
            m_Model->revert();
        }
        QString typeText = type->text();
        setCompletionList(typeText);
        QString textAbstract = abstractEdit->text();
        setCompletionAbstractList(textAbstract);
//        qWarning() << __FILE__ << QString::number(__LINE__) << "error "<< m_Model->lastError().text() ;
    }
}

void MedicalProcedureWidget::on_mpComboBox_currentIndexChanged(int index)
{
    //saveModel();
    m_Mapper->setCurrentIndex(mpComboBox->currentIndex());
}

void MedicalProcedureWidget::on_addButton_clicked()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        Utils::Log::addError(this, "Unable to add row", __FILE__, __LINE__);
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    mpComboBox->setCurrentIndex(m_Model->rowCount()-1);
    rateSpin->setValue(70.00);
    rateSpin->setFocus();
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setFocus();
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
}

void MedicalProcedureWidget::on_save_clicked()
{
    saveModel();
}

//void MedicalProcedureWidget::on_name_textChanged(const QString & text){
//    mpComboBox->setItemText(mpComboBox->currentIndex(),text);
//}

void MedicalProcedureWidget::on_removeButton_clicked()
{
    if (!m_Model->removeRow(mpComboBox->currentIndex()))
    {
    	  Utils::Log::addError(this, "Unable to remove row", __FILE__, __LINE__);
        }
    mpComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void MedicalProcedureWidget::saveToSettings(Core::ISettings *sets)
{
    if (!m_Model->submit()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("medical_procedures")));
        Utils::warningMessageBox(tr("Can not submit medical procedure to your personnal database."),
                                 tr("An error occured during medical procedures saving. Datas are corrupted."));
    }
        QString typeText = type->text();
        setCompletionList(typeText);
        QString textAbstract = abstractEdit->text();
        setCompletionAbstractList(textAbstract);
        connect(name,SIGNAL(textEdited(const QString &)),mpComboBox,SLOT(setEditText(const QString &)));
        update();
}

void MedicalProcedureWidget::writeDefaultSettings(Core::ISettings *s)
{
//    Utils::Log::addMessage("MedicalProcedureWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("MedicalProcedureWidget"));
//    s->sync();
}

void MedicalProcedureWidget::changeEvent(QEvent *e)
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

void MedicalProcedureWidget::setCompletionList(const QString & text){
    m_completionList << text;
    m_completionList.removeDuplicates();
}

void MedicalProcedureWidget::setCompletionAbstractList(const QString & text){
    m_completionAbstractList << text;
    m_completionAbstractList.removeDuplicates();
}

void MedicalProcedureWidget::on_type_textChanged(const QString & text){
    QCompleter *c = new QCompleter(m_completionList,this);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    type->setCompleter(c);
}

void MedicalProcedureWidget::on_abstractEdit_textChanged(const QString & text){
    QCompleter *c = new QCompleter(m_completionAbstractList,this);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    abstractEdit->setCompleter(c);
}

static QString getCsvDefaultFile()
{
    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
    QDir dir(sqlDirPath);
    if (!dir.exists())
        return QString();
    QString fileName = QString("medical_procedure_%1.csv").arg(QLocale().name());
    QFile file(dir.absolutePath() + QDir::separator() + fileName);
    if (!file.exists())
        return QString();
    return file.fileName();
}

QStandardItemModel *MedicalProcedureWidget::MedicalProcedureModelByLocale()
{
    QStandardItemModel *model = new QStandardItemModel;
    QString csvFileName = getCsvDefaultFile();
    qDebug() << __FILE__ << QString::number(__LINE__) << " csvFileName =" << csvFileName ;
    QFile file(getCsvDefaultFile());
    // some validity checking
    if (!file.exists()) {
        LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(QLocale().name() + " " + tr("Medical_procedure.")));
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
    //int row = 0;
    while (!stream.atEnd())
    {
        int row = 0;
        QString line = stream.readLine();
        QStringList listOfSeparators;
        listOfSeparators << ",\"" << ";\"" << QString("\t\"")
                         << ",''" << ";''" << QString("\t''");
        QString separator = ";\"";
        /*QString separatorStr;
        foreach(separatorStr,listOfSeparators){
            if (line.contains(separatorStr)){
                separator = separatorStr;
                }
            }*/
        if (!line.contains("MP_UUID")){
            //"MP_ID","MP_UUID","MP_USER_UID","NAME","ABSTRACT","TYPE","AMOUNT","REIMBOURSEMENT","DATE"
            QList<QStandardItem*> listOfItemsData;
            QStringList listOfItems;
            listOfItems = line.split(separator);
            for (int i = 0; i < AccountDB::Constants::MP_MaxParam ; i += 1){
                //model->setData(model->index(row,i),listOfItems[i],Qt::EditRole);
        	QStandardItem * item = new QStandardItem;
        	//qDebug() << __FILE__ << QString::number(__LINE__) << " listOfItems[i] =" << listOfItems[i] ;
        	QString itemOfList = listOfItems[i];
        	itemOfList.remove("\"");
        	itemOfList.remove("'");
        	item->setData(itemOfList);
        	listOfItemsData << item;
        	}
            model->appendRow(listOfItemsData);
            ++row;  
            qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
            }
    }
    return model;
}


bool MedicalProcedureWidget::fillEmptyMPModel(){
    bool test = false;
    QStandardItemModel * model = MedicalProcedureModelByLocale();
    int availModelRows = model->rowCount();
    //qDebug() << __FILE__ << QString::number(__LINE__) << " availModelRows = " << QString::number(availModelRows) ;
    QString strList;
    for (int i = 0; i < availModelRows; i += 1){
        if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex()))
    	  		{qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(m_Model->rowCount()) ;
    	  			  /*QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to insert row \n")
    	  			  +__FILE__+QString::number(__LINE__),QMessageBox::Ok);*/
    	  		    }
    	  		    QString strValues;
    	  	for (int j = 0; j < AccountDB::Constants::MP_MaxParam ; j += 1){
    	  		QStandardItem * item = model->itemFromIndex(model->index(i,j));
    	  		QVariant value = item->data();
    	  		//todo, semantics
    	  		if (value.canConvert(QVariant::String))
    	  		{
    	  			  QString strValue = value.toString().replace("'","''");
    	  			  value = QVariant::fromValue(strValue);
    	  		    }
    	  		    strValues += value.toString()+" ";
    	  		//qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
    	  		//qDebug() << __FILE__ << QString::number(__LINE__) << "m_Model->rowCount() =" << QString::number(m_Model->rowCount()) ;
    	  		if (!m_Model->setData(m_Model->index(m_Model->rowCount()-1,j),value,Qt::EditRole))
    	  		{
    	  			qWarning() << __FILE__ << QString::number(__LINE__) << "data not inserted !" ;  
    	  		    }
    	  	    }
    	  	    strList += strValues+"\n";
    	      test = m_Model->submit();
    	      }
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " values = \n" << strList;

    return test;
}


