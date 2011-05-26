/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "insurancepage.h"
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
#include <QDir>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
const QString global_resourcesPath = qApp->applicationDirPath()+"/../../global_resources";


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  AccountUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
InsurancePage::InsurancePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("InsurancePage"); }

InsurancePage::~InsurancePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString InsurancePage::id() const { return objectName(); }
QString InsurancePage::name() const { return tkTr(Trans::Constants::INSURANCE); }
QString InsurancePage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void InsurancePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void InsurancePage::applyChanges()
{qDebug() << __FILE__ << QString::number(__LINE__) << " applyChanges ";
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void InsurancePage::finish() { delete m_Widget; }

void InsurancePage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *InsurancePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new InsuranceWidget(parent);
    return m_Widget;
}

InsuranceWidget::InsuranceWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    //QCoreApplication::processEvents(QEventLoop::AllEvents);
    setObjectName("InsuranceWidget");
    setupUi(this);
    m_user_uid = user()->value(Core::IUser::Uuid).toString();
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }
    m_hashTownZip = parseZipcodeCsv();
    addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    addButton->setText("New");
    deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    deleteButton->setText("Delete");
    zipComboBox->setEditable(true);
    zipComboBox->setInsertPolicy(QComboBox::NoInsert);
    
    countryComboBox->setEditable(true);
    countryComboBox->setInsertPolicy(QComboBox::NoInsert);
        
    m_Model = new AccountDB::InsuranceModel(this);
//    if (m_Model->rowCount() < 1)  {
//        if (!fillEmptyAvailableModel()) {
//            QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to fill availablemodel whith local .csv"),
//                                 QMessageBox::Ok);
//        }
//    }
    /** \todo  m_Model->setUserUuid(); */
    m_insuranceUidLabel = new QSpinBox(this);
    //m_insuranceUidLabel->setText("NULL");
    m_insuranceUidLabel->setValue(11111);
    //m_insuranceUidLabel->hide();
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(m_insuranceUidLabel,1);// AccountDB::Constants::INSURANCE_UID);
    m_Mapper->addMapping(nameEdit, AccountDB::Constants::INSURANCE_NAME);
    m_Mapper->addMapping(adressEdit, AccountDB::Constants::INSURANCE_ADRESS);
    m_Mapper->addMapping(cityEdit, AccountDB::Constants::INSURANCE_CITY,"text");
    m_Mapper->addMapping(zipComboBox, AccountDB::Constants::INSURANCE_ZIPCODE,"currentText");
    m_Mapper->addMapping(countryComboBox, AccountDB::Constants::INSURANCE_COUNTRY,"currentText");
    
    m_Mapper->addMapping(phoneEdit, AccountDB::Constants::INSURANCE_TEL);
    m_Mapper->addMapping(faxEdit, AccountDB::Constants::INSURANCE_FAX);
    m_Mapper->addMapping(mailEdit, AccountDB::Constants::INSURANCE_MAIL);
    
    m_Mapper->addMapping(contactEdit, AccountDB::Constants::INSURANCE_CONTACT);
    m_Mapper->toFirst();
    insuranceComboBox->setModel(m_Model);
    insuranceComboBox->setModelColumn(AccountDB::Constants::INSURANCE_NAME);
  
    setDatasToUi();
    connect(zipComboBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(findCityFromZipCode(const QString &)));
}

InsuranceWidget::~InsuranceWidget()
{
    //saveModel();
}

void InsuranceWidget::fillComboBoxes(){
    qDebug() << __FILE__ << QString::number(__LINE__) << " in fillcomboboxes "  ;
    QLocale local;
    QString localCountry;
    localCountry = QLocale::countryToString(local.country());
    qWarning() << __FILE__ << QString::number(__LINE__) << " country =" << localCountry ;
    //zipcodes 
    QStringList listOfZipcodes;
    listOfZipcodes  = m_hashTownZip.keys();
    listOfZipcodes.removeDuplicates();
    listOfZipcodes.sort();
    zipComboBox->addItems(listOfZipcodes);
    //countries
    QStringList listForCountry;
    listForCountry = listOfCountries();
    listForCountry.sort();
    listForCountry.prepend(localCountry);
    countryComboBox->addItems(listForCountry);
    emit findCityFromZipCode(zipComboBox->currentText());    
}

void InsuranceWidget::setDatasToUi()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(insuranceComboBox->currentIndex());
    m_Mapper->setCurrentIndex(insuranceComboBox->currentIndex());
}

void InsuranceWidget::saveModel()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes ?"),
                                          tr("You make changes into the insurance table.\n"
                                             "Do you want to save them ?"));
        if (yes) {
           if (!m_Model->submit()) {qDebug() << __FILE__ << QString::number(__LINE__) << " insurance submit ";
                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("insurance")));
            }
        } 
        else {
            m_Model->revert();
        }
    }
    qDebug() << __FILE__ << QString::number(__LINE__) << " site error =" << m_Model->lastError().text();
}

void InsuranceWidget::on_insuranceComboBox_currentIndexChanged(int index)
{
    //saveModel();
    Q_UNUSED(index);
    m_Mapper->setCurrentIndex(insuranceComboBox->currentIndex());
}

void InsuranceWidget::on_addButton_clicked()
{
    //qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        LOG_ERROR("Unable to add row");
    //qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    insuranceComboBox->setCurrentIndex(m_Model->rowCount()-1);
    m_insuranceUidLabel->setValue(calcInsuranceUid());
    m_insuranceUidLabel->setFocus();
    //qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUidLabel =" << m_insuranceUidLabel->text();
    //qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    /*nameEdit->setText("name");
    adressEdit->setText("adress");
    cityEdit->setText("city");
    countryComboBox->addItem("country");
    countryComboBox->setFocus();
    
    zipComboBox->setFocus();
    phoneEdit->setText("123457");
    faxEdit->setText("1245677");
    mailEdit->setText("DFFJ");
    contactEdit->setText("hkgg");*/

}




void InsuranceWidget::on_deleteButton_clicked()
{
    if (!m_Model->removeRow(insuranceComboBox->currentIndex()))
    {
          LOG_ERROR("Unable to remove row");
        }
    insuranceComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void InsuranceWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    if (!m_Model->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("insurance")));
        Utils::warningMessageBox(tr("Can not submit insurance to your personnal database."),
                                 tr("An error occured during insurance saving. Datas are corrupted."));
    }
        connect(nameEdit,SIGNAL(textEdited(const QString &)),insuranceComboBox,SLOT(setEditText(const QString &)));
        update();
}

void InsuranceWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Utils::Log::addMessage("InsuranceWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("InsuranceWidget"));
//    s->sync();
}

void InsuranceWidget::changeEvent(QEvent *e)
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

void InsuranceWidget::showEvent(QShowEvent *event){
    Q_UNUSED(event);
    qWarning() << __FILE__ << QString::number(__LINE__) << " insurance activated "   ;
    fillComboBoxes();
    update();
}

void InsuranceWidget::findCityFromZipCode(const QString & zipCodeText){
    cityEdit->setFocus();
    QString city = m_hashTownZip.value(zipCodeText);
    cityEdit->setText(city);
}

QHash<QString,QString> InsuranceWidget::parseZipcodeCsv(){
    QHash<QString,QString> hash;
    QString zipcodeStr = global_resourcesPath+"/textfiles/zipcodes.csv";
    QFile zipcodeFile(zipcodeStr);
    if(!zipcodeFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qWarning() << __FILE__ << QString::number(__LINE__) << "zipcode cannot open !" ;
       }
    QTextStream stream(&zipcodeFile);
    while (!stream.atEnd())
    {
    	QString line = stream.readLine();
    	QString line2 = line;
    	if (line.contains("FIN")|| line.contains("<p"))
    	{
    		  break;	
    	    }
    	QString city = line.replace(QRegExp("[0-9]"),"").replace(",","").trimmed();
        QString zip = line2.replace(QRegExp("[^0123456789]"),"").trimmed();

    	   // qDebug() << __FILE__ << QString::number(__LINE__) << " zip city  =" << zip+","+city;
    	    hash.insertMulti(zip,city);//zipcode,city

        }
        qDebug() << __FILE__ << QString::number(__LINE__) << " hash size =" << hash.size();
    return hash;
}

QStringList InsuranceWidget::listOfCountries(){
    QStringList list;
    QString countryFileStr = global_resourcesPath+"/textfiles/pays.txt";
    QFile file(countryFileStr);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "pays.txt cannot open !" ;
    }
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
    	QString line = stream.readLine().trimmed();
    	//qDebug() << __FILE__ << QString::number(__LINE__) << " country =" << line;
        if (!line.isEmpty()) {
            list << line;
        }
    }
    return list;
}

int InsuranceWidget::calcInsuranceUid(){
    QModelIndex index = m_Model->index(m_Model->rowCount()-2,AccountDB::Constants::INSURANCE_UID);
    if (!index.isValid()) {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid" ;
        }
    int siteUidBefore = m_Model->data(index,Qt::DisplayRole).toInt();
    qDebug() << __FILE__ << QString::number(__LINE__) << " siteUidBefore =" << QString::number(siteUidBefore) ;
    int siteUid =  siteUidBefore + 1;
    qDebug() << __FILE__ << QString::number(__LINE__) << " siteUid =" << QString::number(siteUid);
    return siteUid;
}

//static QString getCsvDefaultFile()
//{
//    QString sqlDirPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account";
//    QDir dir(sqlDirPath);
//    if (!dir.exists())
//        return QString();
//    QString fileName = QString("insurances_%1.csv").arg(QLocale().name());
//    QFile file(dir.absolutePath() + QDir::separator() + fileName);
//    if (!file.exists())
//        return QString();
//    return file.fileName();
//}

//QStandardItemModel *InsuranceWidget::insuranceModelByLocale()
//{
//    QStandardItemModel *model = new QStandardItemModel;
//    QString csvFileName = getCsvDefaultFile();
//    qDebug() << __FILE__ << QString::number(__LINE__) << " csvFileName =" << csvFileName ;
//    QFile file(getCsvDefaultFile());
//    // some validity checking
//    if (!file.exists()) {
//        LOG_ERROR(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(QLocale().name() + " " + tr("Insurances")));
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
//        if (!line.contains("INSURANCE_ID")){
//            //"INSURANCE_ID","PARENT","TYPE","LABEL","CODE","COMMENT","DEDUCTIBILITY"
//            QList<QStandardItem*> listOfItemsData;
//            QStringList listOfItems;
//            listOfItems = line.split(separator);
//            for (int i = 0; i < AccountDB::Constants::INSURANCE_MaxParam ; i += 1){
//                //model->setData(model->index(row,i),listOfItems[i],Qt::EditRole);
//        	QStandardItem * item = new QStandardItem;
//        	//qDebug() << __FILE__ << QString::number(__LINE__) << " listOfItems[i] =" << listOfItems[i] ;
//        	QString itemOfList = listOfItems[i];
//        	itemOfList.remove("\"");
//        	itemOfList.remove("'");
//        	item->setData(itemOfList);
//        	listOfItemsData << item;
//        	}
//            model->appendRow(listOfItemsData);
//            ++row;
//            }
//    }
//    return model;
//}

//bool InsuranceWidget::fillEmptyAvailableModel()
//{
//    bool test = false;
//    QStandardItemModel  *model = insuranceModelByLocale();
//    int availModelRows = model->rowCount();
//    //qDebug() << __FILE__ << QString::number(__LINE__) << " availModelRows = " << QString::number(availModelRows) ;
//    QString strList;
//    for (int i = 0; i < availModelRows; i += 1){
//        if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex()))
//    	  		{qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(m_Model->rowCount()) ;
//    	  			  /*QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to insert row \n")
//    	  			  +__FILE__+QString::number(__LINE__),QMessageBox::Ok);*/
//    	  		    }
//    	  		    QString strValues;
//    	  	for (int j = 0; j < AccountDB::Constants::INSURANCE_MaxParam ; j += 1){
//    	  		QStandardItem * item = model->itemFromIndex(model->index(i,j));
//    	  		QVariant value = item->data();
//    	  		//todo, semantics
//    	  		if (value.canConvert(QVariant::String))
//    	  		{
//    	  			  QString strValue = value.toString().replace("'","''");
//    	  			  value = QVariant::fromValue(strValue);
//    	  		    }
//    	  		    strValues += value.toString()+" ";
//    	  		//qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
//    	  		//qDebug() << __FILE__ << QString::number(__LINE__) << "m_Model->rowCount() =" << QString::number(m_Model->rowCount()) ;
//    	  		if (!m_Model->setData(m_Model->index(m_Model->rowCount()-1,j),value,Qt::EditRole))
//    	  		{
//    	  			qWarning() << __FILE__ << QString::number(__LINE__) << "data not inserted !" ;
//    	  		    }
//    	  	    }
//    	  	    strList += strValues+"\n";
//    	      test = m_Model->submit();
//    	      }
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " values = \n" << strList;

//    return test;
//}

