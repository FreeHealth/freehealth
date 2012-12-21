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
#include "accountpreferences.h"

#include <accountplugin/constants.h>

#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/accountbase.h>

#include <accountbaseplugin/datapackmodel.h>
#include <accountbaseplugin/medicalproceduremodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>
#include <printerplugin/textdocumentextra.h>

#include <QPixmap>
#include <QSqlRecord>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline AccountDB::AccountBase *base() {return AccountDB::AccountBase::instance();}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  AccountUserOptionsPage  //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
AccountUserOptionsPage::AccountUserOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("AccountUserOptionsPage"); }

AccountUserOptionsPage::~AccountUserOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString AccountUserOptionsPage::id() const { return objectName(); }
QString AccountUserOptionsPage::displayName() const { return tkTr(Trans::Constants::USER); }
QString AccountUserOptionsPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int AccountUserOptionsPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 1;
}

void AccountUserOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void AccountUserOptionsPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AccountUserOptionsPage::finish() { delete m_Widget; }

void AccountUserOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *AccountUserOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AccountUserWidget(parent);
    return m_Widget;
}

AccountUserWidget::AccountUserWidget(QWidget *parent) :
        QWidget(parent), header(0), footer(0), wm(0)
{
    setObjectName("AccountUserWidget");
    header = new Print::TextDocumentExtra;
    footer = new Print::TextDocumentExtra;
    wm = new Print::TextDocumentExtra;
    setupUi(this);
    previewer = Print::Printer::previewer(this);
    userLayout->addWidget(previewer);
    setDataToUi();
}

void AccountUserWidget::setDataToUi()
{
    header = Print::TextDocumentExtra::fromXml(settings()->value(Account::Constants::S_USER_HEADER).toString());
    footer = Print::TextDocumentExtra::fromXml(settings()->value(Account::Constants::S_USER_FOOTER).toString());
    wm = Print::TextDocumentExtra::fromXml(settings()->value(Account::Constants::S_USER_WATERMARK).toString());

    previewer->setHeader(header);
    previewer->setFooter(footer);
    previewer->setWatermark(wm);
}

void AccountUserWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    Print::TextDocumentExtra *tmp = new Print::TextDocumentExtra;

    previewer->headerToPointer(tmp);
    s->setValue(Account::Constants::S_USER_HEADER, tmp->toXml());

    previewer->footerToPointer(tmp);
    s->setValue(Account::Constants::S_USER_FOOTER, tmp->toXml());

    previewer->watermarkToPointer(tmp);
    s->setValue(Account::Constants::S_USER_WATERMARK, tmp->toXml());
}

void AccountUserWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
//    qWarning() << "---------> writedefaults";
//    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("AccountUserWidget"));
//    s->sync();
}

void AccountUserWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  AccountDatabaseDefaultsPage  ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
AccountDatabaseDefaultsPage::AccountDatabaseDefaultsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("AccountDatabaseDefaultsPage");
}

AccountDatabaseDefaultsPage::~AccountDatabaseDefaultsPage()
{
    if (m_Widget) delete m_Widget;
        m_Widget = 0;
}

QString AccountDatabaseDefaultsPage::id() const { return objectName(); }
QString AccountDatabaseDefaultsPage::displayName() const { return tkTr(Trans::Constants::DEFAULTS); }
QString AccountDatabaseDefaultsPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }
int AccountDatabaseDefaultsPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_ACCOUNT + 10;
}

void AccountDatabaseDefaultsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void AccountDatabaseDefaultsPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AccountDatabaseDefaultsPage::finish() { delete m_Widget; }

void AccountDatabaseDefaultsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *AccountDatabaseDefaultsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AccountDatabaseDefaultsWidget(parent);
    return m_Widget;
}

AccountDatabaseDefaultsWidget::AccountDatabaseDefaultsWidget(QWidget *parent) :
        QWidget(parent)
{
    setObjectName("AccountDatabaseDefaultsWidget");
    setupUi(this);
    others->hide();
    datapackButton->hide();
//    setDataToUi();
}

void AccountDatabaseDefaultsWidget::on_createButton_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    bool success = true;
    if (medicalProcedure->isChecked()) {
        if (!createDefaultsFor("medical_procedure", AccountDB::Constants::Table_MedicalProcedure))
            {
                Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Medical procedure defaults can not be included."));
                success = false;
             if (!createMinimalsDefaults(AccountDB::Constants::Table_MedicalProcedure))
             {
                Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Medical procedure minimal defaults can not be included."));
                success = false;
                 }
             else{
                 Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Only few medical procedures are included.\nYou have to complete them yourself."), QString(), tr("Minimal defaults.") );
                 }
             }
    }
    if (assetsRates->isChecked()) {
        if (!createDefaultsFor("assets_rates", AccountDB::Constants::Table_AssetsRates))
            {Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Assets Rates defaults can not be included."));
             success = false;
             }
    }
    if (available_movements->isChecked()) {
        if (!createDefaultsFor("available_movement", AccountDB::Constants::Table_AvailableMovement))
            {Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Movements defaults can not be included."));
             success = false;
             }
    }
    if (distance->isChecked()) {
        if (!createDefaultsFor("distance_rules", AccountDB::Constants::Table_DistanceRules))
            {Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Distance rules defaults can not be included."));
             success = false;
             }
    }
    if (insurance->isChecked()) {
        if (!createDefaultsFor("insurances", AccountDB::Constants::Table_Insurance))
            {Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Insurance defaults can not be included."));
             success = false;
             }
    }
//    if (others->isChecked()) {
//        if (!createDefaultsFor("assets_rates", AccountDB::Constants::Table_AssetsRates))
//            {Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Assets Rates defaults can not be included."));
//             success = false;
//              }
//    }
    QApplication::restoreOverrideCursor ();
    if (success) {
          qWarning() << __FILE__ << QString::number(__LINE__) << "Defaults have been created." ;
          Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Defaults have been created."));
    }
}

void AccountDatabaseDefaultsWidget::setDataToUi()
{
    // TODO: For each defaults checkbox: check if defaults are: 1) available (if no -> unable checkbox) 2) not already inserted (unable checkbox)
}

void AccountDatabaseDefaultsWidget::saveToSettings(Core::ISettings *)
{
}

void AccountDatabaseDefaultsWidget::writeDefaultSettings(Core::ISettings *)
{
}

void AccountDatabaseDefaultsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

// just send "assets_rates" -> "/abs/path/assets_rates_fr_FR.csv"
static QString getCsvAbsFilePath(const QString &filePrototype)
{
    QString sqlPath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account/";
    return QString ("%1%2_%3.csv").arg(sqlPath).arg(filePrototype).arg(QLocale().name());
}

bool AccountDatabaseDefaultsWidget::createDefaultsFor(const QString &filePrototype, const int tableRef)
{
    bool yes = Utils::Database::importCsvToDatabase(AccountDB::Constants::DB_ACCOUNTANCY,
                                         getCsvAbsFilePath(filePrototype),
                                         base()->table(tableRef),
                                         ";", true);
    return yes;
}

bool AccountDatabaseDefaultsWidget::createMinimalsDefaults(const int tableRef)
{
    bool success = true;
    QStringList valuesList;
    valuesList <<  "'1';'{07262c6f-9d08-4208-ae74-ba9b7d74daea}';'{00000000-0000-0000-0000-000000000000}';'2';'C';'consultation';'NGAP';'1';'70';'2012-06-22';'NULL';'FR'"
               <<  "'2';'{78521164-5ea9-4dcf-926f-b0518fcbf580}';'{00000000-0000-0000-0000-000000000000}';'2';'MNO';'majoration pour les enfants de 0 à 2 ans';'Forfaits';'5';'70';'2012-06-22';'NULL';'FR'"
               <<  "'3';'{f8593736-b517-4098-847e-f7c6cc15e051}';'{00000000-0000-0000-0000-000000000000}';'2';'DEQP003';'Électrocardiographie sur au moins 12 dérivations';'CCAM';'13';'70';'2012-06-22';'<?xml version=1.0 encoding=ISO-8859-1?><activity>1</activity><phase>0</phase><reimbursment></reimbursment><agreement></agreement><exemption>2</exemption><regroupment>ATM</regroupment>';'FR'";

    success = createMinimalDefaultsFor(AccountDB::Constants::DB_ACCOUNTANCY,tableRef,valuesList);
    return success;
}

bool AccountDatabaseDefaultsWidget::createMinimalDefaultsFor(const QString &connectionName,const int tableRef, const QStringList & valuesList)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                          .arg(db.connectionName(), db.lastError().text()));
            return false;
            }
        }
    if (!db.transaction())
    {
          LOG_FOR("Account","Database cannot support transaction");
          return false;
        }
    foreach(QString line , valuesList){
        QStringList values = line.split(";",QString::KeepEmptyParts);
        QString req = base()->prepareInsertQuery(tableRef);
        QSqlQuery query(db);
        for (int col = 0; col < values.size(); ++col)
        {
            query.bindValue(col,values[col]);
            }
        if (!query.exec(req))
        {
              LOG_QUERY_ERROR(query);
              if (!db.rollback())
              {
                  LOG_FOR("Account","Unable to roll back the transaction.");
                  return false;
                  }
              return false;
            }
        }
    if (!db.commit())
    {
          LOG_FOR("Account",db.lastError().text());
          return false;
        }
    return true;
}


/*void AccountDatabaseDefaultsWidget::on_datapackButton_clicked()
{
        LOG("datapackButton clicked");
        AccountDB::DatapackMPModel dtpkmodel(this);
        AccountDB::MedicalProcedureModel MPmodel(this);
        int MPmodelRowCount = MPmodel.rowCount();
        int dtpkRowCount = dtpkmodel.rowCount();
        qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(MPmodelRowCount)
                 << QString::number(dtpkRowCount) ;
        //test of date
        QList<QDate> listOfDates;
        for (int r = 0; r < MPmodelRowCount; ++r)
        {
              QDate date = MPmodel.data(MPmodel.index(r,AccountDB::Constants::MP_DATE),Qt::DisplayRole).toDate();
              listOfDates << date;
            }
        QDate dateDapapack = dtpkmodel.data(dtpkmodel.index(1,AccountDB::Constants::MP_DATE),Qt::DisplayRole).toDate();
        if (listOfDates.contains(dateDapapack))
        {
              LOG_ERROR("Datapack has already been loaded.");
              Utils::warningMessageBox( tr("Warning"), tr("Datapack has already been loaded."));
              return;
            }
        if (!MPmodel.insertRows(MPmodelRowCount,dtpkRowCount,QModelIndex()))
        {
              LOG_ERROR(MPmodel.lastError().text());
            }
        for (int row = 0; row < dtpkRowCount ; ++row)
        {
             for (int col = 0; col < dtpkmodel.columnCount(); ++col)
             {
                  QVariant data = dtpkmodel.data(dtpkmodel.index(row,col),Qt::DisplayRole);
                  if (!MPmodel.setData(MPmodel.index(MPmodelRowCount+row,col),data,Qt::EditRole))
                  {
                      LOG_ERROR(MPmodel.lastError().text());
                      }
                 }
            }

}*/
