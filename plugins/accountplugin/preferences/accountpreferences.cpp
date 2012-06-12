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

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>
#include <printerplugin/textdocumentextra.h>

#include <QPixmap>

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
QString AccountUserOptionsPage::name() const { return tkTr(Trans::Constants::USER); }
QString AccountUserOptionsPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void AccountUserOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void AccountUserOptionsPage::applyChanges()
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
    setDatasToUi();
}

void AccountUserWidget::setDatasToUi()
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
///////////////////////////////  AccountDatabaseDefautsPage  ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
AccountDatabaseDefautsPage::AccountDatabaseDefautsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("AccountDatabaseDefautsPage"); }

AccountDatabaseDefautsPage::~AccountDatabaseDefautsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString AccountDatabaseDefautsPage::id() const { return objectName(); }
QString AccountDatabaseDefautsPage::name() const { return tkTr(Trans::Constants::DEFAULTS); }
QString AccountDatabaseDefautsPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void AccountDatabaseDefautsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void AccountDatabaseDefautsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AccountDatabaseDefautsPage::finish() { delete m_Widget; }

void AccountDatabaseDefautsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *AccountDatabaseDefautsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AccountDatabaseDefautsWidget(parent);
    return m_Widget;
}

AccountDatabaseDefautsWidget::AccountDatabaseDefautsWidget(QWidget *parent) :
        QWidget(parent)
{
    setObjectName("AccountDatabaseDefautsWidget");
    setupUi(this);
    others->hide();
//    setDatasToUi();
}

void AccountDatabaseDefautsWidget::on_createButton_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    bool success = true;
    if (medicalProcedure->isChecked()) {
        if (!createDefaultsFor("medical_procedure", AccountDB::Constants::Table_MedicalProcedure))
            {Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Medical procedure defaults can not be included."));
             success = false;
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

void AccountDatabaseDefautsWidget::setDatasToUi()
{
    // TODO: For each defaults checkbox: check if defaults are: 1) available (if no -> unable checkbox) 2) not already inserted (unable checkbox)
}

void AccountDatabaseDefautsWidget::saveToSettings(Core::ISettings *)
{
}

void AccountDatabaseDefautsWidget::writeDefaultSettings(Core::ISettings *)
{
}

void AccountDatabaseDefautsWidget::changeEvent(QEvent *e)
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

bool AccountDatabaseDefautsWidget::createDefaultsFor(const QString &filePrototype, const int tableRef)
{
    bool yes = Utils::Database::importCsvToDatabase(AccountDB::Constants::DB_ACCOUNTANCY,
                                         getCsvAbsFilePath(filePrototype),
                                         base()->table(tableRef),
                                         ";", true);
    return yes;
}

void AccountDatabaseDefautsWidget::on_datapackButton_clicked()
{
        QSqlDatabase db = QSqlDatabase::database(AccountDB::Constants::DATAPACK_ACCOUNTANCY);
        AccountDB::DatapackMPModel dtpk(this);
        AccountDB::MedicalProcedureModel MP(this);
        /*
        1) vérifier que la mise à jour n'est pas déjà faite.
        2) updater les valeurs communes 
        3) rajouter les nouvelles valeurs.
        */
}
