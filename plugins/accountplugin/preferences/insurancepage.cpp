/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "insurancepage.h"
#include <accountplugin/constants.h>
#include <accountbaseplugin/constants.h>

#include <zipcodesplugin/zipcodescompleters.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include "ui_insurancepage.h"

#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QRegExp>
#include <QLocale>
#include <QDir>

enum { WarnDebugMessage = false };

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
const QString global_resourcesPath = qApp->applicationDirPath()+"/../../global_resources";


InsurancePage::InsurancePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("InsurancePage");
}

InsurancePage::~InsurancePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString InsurancePage::id() const { return objectName(); }
QString InsurancePage::displayName() const { return tkTr(Trans::Constants::INSURANCE); }
QString InsurancePage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int InsurancePage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 60;
}

void InsurancePage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void InsurancePage::apply()
{
    if (WarnDebugMessage)
        LOG("applyChanges");
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
    // Setup UI
    ui = new Ui::InsuranceWidget;
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->addButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    ui->deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));

    // Manage current user
    m_user_uid = user()->uuid();
    m_user_fullName = user()->value(Core::IUser::FullName).toString();
    if (m_user_fullName.isEmpty()) {
        m_user_fullName = "Admin_Test";
    }

    // Add zipcodes auto-completer
    ZipCodes::ZipCountryCompleters *c = new ZipCodes::ZipCountryCompleters(this);
    c->setZipLineEdit(ui->zip);
    c->setCityLineEdit(ui->cityEdit);
    c->setCountryComboBox(ui->countryComboBox);

    m_Model = new AccountDB::InsuranceModel(this);

    // TODO:  m_Model->setUserUuid();
    m_insuranceUidLabel = new QSpinBox(this);
    //m_insuranceUidLabel->setText("NULL");
    m_insuranceUidLabel->setValue(11111);
    if (Utils::isReleaseCompilation())
        m_insuranceUidLabel->hide();

    // Create mapper
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    m_Mapper->addMapping(m_insuranceUidLabel, AccountDB::Constants::INSURANCE_UID);
    m_Mapper->addMapping(ui->nameEdit, AccountDB::Constants::INSURANCE_NAME);
    m_Mapper->addMapping(ui->adressEdit, AccountDB::Constants::INSURANCE_ADRESS);
    m_Mapper->addMapping(ui->cityEdit, AccountDB::Constants::INSURANCE_CITY,"text");
    m_Mapper->addMapping(ui->zip, AccountDB::Constants::INSURANCE_ZIPCODE, "currentText");
    m_Mapper->addMapping(ui->countryComboBox, AccountDB::Constants::INSURANCE_COUNTRY, "currentIsoCountry");

    m_Mapper->addMapping(ui->phoneEdit, AccountDB::Constants::INSURANCE_TEL);
    m_Mapper->addMapping(ui->faxEdit, AccountDB::Constants::INSURANCE_FAX);
    m_Mapper->addMapping(ui->mailEdit, AccountDB::Constants::INSURANCE_MAIL);

    m_Mapper->addMapping(ui->contactEdit, AccountDB::Constants::INSURANCE_CONTACT);
    m_Mapper->toFirst();
    ui->insuranceComboBox->setModel(m_Model);
    ui->insuranceComboBox->setModelColumn(AccountDB::Constants::INSURANCE_NAME);

    setDataToUi();
}

InsuranceWidget::~InsuranceWidget()
{
    delete ui;
    //saveModel();
}

//void InsuranceWidget::fillComboBoxes()
//{
//    if (WarnDebugMessage)
//        LOG("fillcomboboxes");
//    QLocale local;
//    QString localCountry;
//    localCountry = QLocale::countryToString(local.country());
//    if (WarnDebugMessage)
//        qWarning() << __FILE__ << QString::number(__LINE__) << " country =" << localCountry ;
//    //zipcodes
//    QStringList listOfZipcodes;
//    listOfZipcodes  = m_hashTownZip.keys();
//    listOfZipcodes.removeDuplicates();
//    listOfZipcodes.sort();
//    zipComboBox->addItems(listOfZipcodes);
//    //countries
//    QStringList listForCountry;
//    listForCountry = listOfCountries();
//    listForCountry.sort();
//    listForCountry.prepend(localCountry);
//    countryComboBox->addItems(listForCountry);
//    emit findCityFromZipCode(zipComboBox->currentText());
//}

void InsuranceWidget::setDataToUi()
{
    if (WarnDebugMessage)
        LOG("index row  = " + QString::number(ui->insuranceComboBox->currentIndex()));
    m_Mapper->setCurrentIndex(ui->insuranceComboBox->currentIndex());
}

void InsuranceWidget::saveModel()
{
    if (WarnDebugMessage)
        LOG("currentIndex = " + QString::number(m_Mapper->currentIndex()));
    if (m_Model->isDirty()) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes?"),
                                          tr("You make changes into the insurance table.\n"
                                             "Do you want to save them?"));
        if (yes) {
            if (!m_Model->submit()) {
                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                          arg(tr("insurance")));
            }
        }
        else {
            m_Model->revert();
        }
    }
    if (WarnDebugMessage)
        LOG(" site error = " + m_Model->lastError().text());
}

void InsuranceWidget::on_insuranceComboBox_currentIndexChanged(int index)
{
    //saveModel();
    Q_UNUSED(index);
    m_Mapper->setCurrentIndex(ui->insuranceComboBox->currentIndex());
}

void InsuranceWidget::on_addButton_clicked()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount1 =" << QString::number(m_Model->rowCount());

    if (!m_Model->insertRow(m_Model->rowCount()))
        LOG_ERROR("Unable to add row");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount2 =" << QString::number(m_Model->rowCount());
    ui->insuranceComboBox->setCurrentIndex(m_Model->rowCount()-1);
    m_insuranceUidLabel->setValue(calcInsuranceUid());
    m_insuranceUidLabel->setFocus();
    if (WarnDebugMessage) {
        qDebug() << __FILE__ << QString::number(__LINE__) << " m_insuranceUidLabel =" << m_insuranceUidLabel->text();
        qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());
    }
    /*nameEdit->setText("name");
    adressEdit->setText("address");
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
    if (!m_Model->removeRow(ui->insuranceComboBox->currentIndex())) {
        LOG_ERROR("Unable to remove row");
    }
    ui->insuranceComboBox->setCurrentIndex(m_Model->rowCount() - 1);
}

void InsuranceWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    if (!m_Model->submit()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("insurance")));
        Utils::warningMessageBox(tr("Can not submit insurance to your personnal database."),
                                 tr("An error occured during insurance saving. Data are corrupted."));
    }
    connect(ui->nameEdit,SIGNAL(textEdited(const QString &)), ui->insuranceComboBox,SLOT(setEditText(const QString &)));
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
        ui->retranslateUi(this);
        ui->addButton->setToolTip(tkTr(Trans::Constants::FILENEW_TEXT));
        ui->deleteButton->setToolTip(tkTr(Trans::Constants::REMOVE_TEXT));
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

//void InsuranceWidget::showEvent(QShowEvent *event)
//{
//    Q_UNUSED(event);
//    if (WarnDebugMessage)
//        LOG("insurance activated");
//    fillComboBoxes();
//    update();
//}

//void InsuranceWidget::findCityFromZipCode(const QString & zipCodeText)
//{
////    cityEdit->setFocus();
//    QString city = m_hashTownZip.value(zipCodeText);
//    cityEdit->setText(city);
//}

//QHash<QString,QString> InsuranceWidget::parseZipcodeCsv()
//{
//    QHash<QString,QString> hash;
//    QString zipcodeStr = global_resourcesPath+"/textfiles/zipcodes.csv";
//    QFile zipcodeFile(zipcodeStr);
//    if(!zipcodeFile.open(QIODevice::ReadOnly|QIODevice::Text)){
//        qWarning() << __FILE__ << QString::number(__LINE__) << "zipcode cannot open !" ;
//       }
//    QTextStream stream(&zipcodeFile);
//    while (!stream.atEnd())
//    {
//    	QString line = stream.readLine();
//    	QString line2 = line;
//    	if (line.contains("FIN")|| line.contains("<p"))
//    	{
//    		  break;
//    	    }
//    	QString city = line.replace(QRegExp("[0-9]"),"").replace(",","").trimmed();
//        QString zip = line2.replace(QRegExp("[^0123456789]"),"").trimmed();

//    	    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " zip city  =" << zip+","+city;
//    	    hash.insertMulti(zip,city);//zipcode,city

//        }
//        if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " hash size =" << hash.size();
//    return hash;
//}

//QStringList InsuranceWidget::listOfCountries(){
//    QStringList list;
//    QString countryFileStr = global_resourcesPath+"/textfiles/pays.txt";
//    QFile file(countryFileStr);
//    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
//        qWarning() << __FILE__ << QString::number(__LINE__) << "pays.txt cannot open !" ;
//    }
//    QTextStream stream(&file);
//    while (!stream.atEnd())
//    {
//    	QString line = stream.readLine().trimmed();
//    	if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " country =" << line;
//        if (!line.isEmpty()) {
//            list << line;
//        }
//    }
//    return list;
//}

int InsuranceWidget::calcInsuranceUid(){
    QModelIndex index = m_Model->index(m_Model->rowCount()-2,AccountDB::Constants::INSURANCE_UID);
    if (!index.isValid()) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid" ;
    }
    int siteUidBefore = m_Model->data(index,Qt::DisplayRole).toInt();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " siteUidBefore =" << QString::number(siteUidBefore) ;
    int siteUid =  siteUidBefore + 1;
    if (WarnDebugMessage)
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
//    if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " csvFileName =" << csvFileName ;
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
//        	//if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " listOfItems[i] =" << listOfItems[i] ;
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
//    //if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " availModelRows = " << QString::number(availModelRows) ;
//    QString strList;
//    for (int i = 0; i < availModelRows; i += 1){
//        if (!m_Model->insertRows(m_Model->rowCount(),1,QModelIndex()))
//    	  		{qWarning() << __FILE__ << QString::number(__LINE__) << QString::number(m_Model->rowCount()) ;
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
//    	  		//if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
//    	  		//if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << "m_Model->rowCount() =" << QString::number(m_Model->rowCount()) ;
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

