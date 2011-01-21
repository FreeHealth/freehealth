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




using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }


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
    QStringList listOfZipcodes;
    listOfZipcodes  = m_hashTownZip.keys();
    listOfZipcodes.removeDuplicates();
    listOfZipcodes.sort();
    
    QStringList listForCountry = listOfCountries();
    listForCountry.sort();
    
    zipComboBox->addItems(listOfZipcodes);
    
    countryComboBox->setEditable(true);
    countryComboBox->setInsertPolicy(QComboBox::NoInsert);
    countryComboBox->addItems(listForCountry);
    
    m_Model = new AccountDB::InsuranceModel(this);
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
    //hash of towns and zipcode
    
    setDatasToUi();
    emit findCityFromZipCode(zipComboBox->currentText());
    connect(zipComboBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(findCityFromZipCode(const QString &)));
}

InsuranceWidget::~InsuranceWidget()
{
    saveModel();
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
                Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("insurance")), __FILE__, __LINE__);
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
    m_Mapper->setCurrentIndex(insuranceComboBox->currentIndex());
}

void InsuranceWidget::on_addButton_clicked()
{
    //qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());
    if (!m_Model->insertRow(m_Model->rowCount()))
        Utils::Log::addError(this, "Unable to add row", __FILE__, __LINE__);
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
    	  Utils::Log::addError(this, "Unable to remove row", __FILE__, __LINE__);
        }
    insuranceComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void InsuranceWidget::saveToSettings(Core::ISettings *sets)
{
    if (!m_Model->submit()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("insurance")));
        Utils::warningMessageBox(tr("Can not submit insurance to your personnal database."),
                                 tr("An error occured during insurance saving. Datas are corrupted."));
    }
        connect(nameEdit,SIGNAL(textEdited(const QString &)),insuranceComboBox,SLOT(setEditText(const QString &)));
        update();
}

void InsuranceWidget::writeDefaultSettings(Core::ISettings *s)
{
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

void InsuranceWidget::findCityFromZipCode(const QString & zipCodeText){
    cityEdit->setFocus();
    QString city = m_hashTownZip.value(zipCodeText);
    cityEdit->setText(city);
}

QHash<QString,QString> InsuranceWidget::parseZipcodeCsv(){
    QHash<QString,QString> hash;
    QString zipcodeStr = qApp->applicationDirPath()+"/../global_resources/textfiles/zipcodes.csv";
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
    QString countryFileStr = qApp->applicationDirPath()+"/../global_resources/textfiles/pays.txt";
    QFile file(countryFileStr);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "pays.txt cannot open !" ;
        }
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
    	QString line = stream.readLine().trimmed();
    	//qDebug() << __FILE__ << QString::number(__LINE__) << " country =" << line;
    	if (!line.isEmpty())
    	{
    		list << line;  
    	    }
    }
    return list;
}

int InsuranceWidget::calcInsuranceUid(){
    QModelIndex index = m_Model->index(m_Model->rowCount()-2,AccountDB::Constants::INSURANCE_UID);
    if (!index.isValid())
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid" ;
        }
    int siteUidBefore = m_Model->data(index,Qt::DisplayRole).toInt();
    qDebug() << __FILE__ << QString::number(__LINE__) << " siteUidBefore =" << QString::number(siteUidBefore) ;
    int siteUid =  siteUidBefore + 1;
    qDebug() << __FILE__ << QString::number(__LINE__) << " siteUid =" << QString::number(siteUid);
    return siteUid;
}
