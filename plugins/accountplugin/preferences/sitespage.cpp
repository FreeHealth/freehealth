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
#include "sitespage.h"

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

#include <coreplugin/constants_icons.h>

#include "ui_sitespage.h"

#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QRegExp>
#include <QLocale>

enum { WarnDebugMessage = true };

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
SitesPage::SitesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("SitesPage"); }

SitesPage::~SitesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString SitesPage::id() const { return objectName(); }
QString SitesPage::displayName() const { return tkTr(Trans::Constants::SITES); }
QString SitesPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int SitesPage::sortIndex() const {return 80;}

void SitesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void SitesPage::apply()
{
    if (WarnDebugMessage)
        LOG("applyChanges");
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void SitesPage::finish() { delete m_Widget; }

void SitesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *SitesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new SitesWidget(parent);
    return m_Widget;
}

SitesWidget::SitesWidget(QWidget *parent) :
        QWidget(parent), m_Model(0), m_Mapper(0)
{
    setObjectName("SitesWidget");
    // Setup UI
    ui = new Ui::SitesWidget;
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    // Manage user
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

    // Create mapper
    m_Model = new AccountDB::WorkingPlacesModel(this);
    // TODO:  m_Model->setUserUuid();
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->setModel(m_Model);
    m_Mapper->setCurrentModelIndex(QModelIndex());
    //m_Mapper->addMapping(ui->mpIDLabel, AccountDB::Constants::SITE_ID, "ID");
    m_Mapper->addMapping(ui->nameEdit, AccountDB::Constants::SITES_NAME);
    m_Mapper->addMapping(ui->adressEdit, AccountDB::Constants::SITES_ADRESS);
    m_Mapper->addMapping(ui->cityEdit, AccountDB::Constants::SITES_CITY, "text");
    m_Mapper->addMapping(ui->zip, AccountDB::Constants::SITES_ZIPCODE, "text");
    m_Mapper->addMapping(ui->countryComboBox, AccountDB::Constants::SITES_COUNTRY,"currentIsoCountry");

    m_Mapper->addMapping(ui->phoneEdit, AccountDB::Constants::SITES_TEL);
    m_Mapper->addMapping(ui->faxEdit, AccountDB::Constants::SITES_FAX);
    m_Mapper->addMapping(ui->mailEdit, AccountDB::Constants::SITES_MAIL);

    m_Mapper->addMapping(ui->contactEdit, AccountDB::Constants::SITES_CONTACT);
    m_Mapper->toFirst();

    ui->addRemoveCombo->setModel(m_Model);
    ui->addRemoveCombo->setModelColumn(AccountDB::Constants::SITES_NAME);
    ui->addRemoveCombo->setLabel(tr("&Working places:"));

    //hash of towns and zipcode
    connect(ui->nameEdit, SIGNAL(textEdited(QString)), ui->addRemoveCombo, SLOT(setEditText(QString)));
    connect(ui->addRemoveCombo, SIGNAL(itemAdded(QModelIndex)), ui->nameEdit, SLOT(setFocus()));
    connect(ui->addRemoveCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUi()));
    connect(ui->addRemoveCombo, SIGNAL(currentIndexChanged(int)), m_Mapper, SLOT(setCurrentIndex(int)));
    setDataToUi();
    updateUi();
}

SitesWidget::~SitesWidget()
{
    delete ui;
    //saveModel();
}

//void SitesWidget::fillHugeWidgets(){
//    QStringList listOfZipcodes;
//    listOfZipcodes  = m_hashTownZip.keys();
//    listOfZipcodes.removeDuplicates();
//    listOfZipcodes.sort();
//    QLocale local;
//    QString localCountry;
//    localCountry = QLocale::countryToString(local.country());
//    if (WarnDebugMessage)
//        qDebug() << __FILE__ << QString::number(__LINE__) << " country =" << localCountry ;
//    QStringList listForCountry;
//    listForCountry = listOfCountries();
//    listForCountry.sort();
//    listForCountry.prepend(localCountry);

//    zipComboBox->addItems(listOfZipcodes);

//    countryComboBox->addItems(listForCountry);
//}

void SitesWidget::setDataToUi()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "index row  =" << QString::number(ui->addRemoveCombo->currentIndex());
    m_Mapper->setCurrentIndex(ui->addRemoveCombo->currentIndex());
}

void SitesWidget::saveModel()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " currentIndex =" << QString::number(m_Mapper->currentIndex());

    // FIXME: here you should have to check all cols isDirty state of the row
    if (m_Model->isDirty(m_Model->index(m_Mapper->currentIndex(),0))) {
        bool yes = Utils::yesNoMessageBox(tr("Save changes?"),
                                          tr("You make changes into the sites table.\n"
                                             "Do you want to save them?"));
        if (yes) {
           if (!m_Model->submitAll()) {
                LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).
                                                   arg(tr("sites")));
            }
        }
        else {
            m_Model->revert();
        }
    }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " site error =" << m_Model->lastError().text();
}

void SitesWidget::on_addRemoveCombo_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    //saveModel();
    m_Mapper->setCurrentIndex(ui->addRemoveCombo->currentIndex());
}

void SitesWidget::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
    if (!m_Model->submitAll()) {
        LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_SAVE_DATA_IN_DATABASE_1).arg(tr("sites")));
        Utils::warningMessageBox(tr("Cannot submit sites to your personal database."),
                                 tr("An error occured during sites saving. Data are corrupted."));
    }
    update();
}

void SitesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    Utils::Log::addMessage("SitesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("SitesWidget"));
//    s->sync();
}

void SitesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
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

//void SitesWidget::showEvent(QShowEvent *event){
//    Q_UNUSED(event);
//    if (WarnDebugMessage)
//        qWarning() << __FILE__ << QString::number(__LINE__) << " sites activated "   ;
//    fillHugeWidgets();
//    update();
//}

//void SitesWidget::findCityFromZipCode(const QString & zipCodeText){
//    cityEdit->setFocus();
//    QString city = m_hashTownZip.value(zipCodeText);
//    cityEdit->setText(city);
//}

//QHash<QString,QString> SitesWidget::parseZipcodeCsv(){
//    QHash<QString,QString> hash;
//    QString zipcodeStr = global_resourcesPath+"/textfiles/zipcodes.csv";
//    QFile zipcodeFile(zipcodeStr);
//    if(!zipcodeFile.open(QIODevice::ReadOnly|QIODevice::Text)){
//        qWarning() << __FILE__ << QString::number(__LINE__) << "zipcode cannot open !" ;
//        if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " zipcodeFile =" << zipcodeStr ;
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

//    	   if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " zip city  =" << zip+","+city;
//    	    hash.insertMulti(zip,city);//zipcode,city

//        }
//        if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " hash size =" << hash.size();
//    return hash;
//}

//QStringList SitesWidget::listOfCountries()
//{
//    QStringList list;
//    QString countryFileStr = global_resourcesPath+"/textfiles/pays.txt";
//    QFile file(countryFileStr);
//    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
//        qWarning() << __FILE__ << QString::number(__LINE__) << "pays.txt cannot open !" ;
//        if (WarnDebugMessage)
//    	      qDebug() << __FILE__ << QString::number(__LINE__) << " pays.txt =" << countryFileStr ;
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


void SitesWidget::updateUi()
{
    ui->tabWidget->setEnabled(ui->addRemoveCombo->currentIndex() != -1);
}
