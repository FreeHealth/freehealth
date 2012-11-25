/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "databaseselectorwidget.h"
#include "ui_databaseselectorwidget.h"
#include "constants.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_current.h>

#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>

using namespace DrugsWidget;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugsDatabaseSelector *selector() {return DrugsDB::DrugsDatabaseSelector::instance();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  DrugsDatabaseSelectorPage  /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsDatabaseSelectorPage::DrugsDatabaseSelectorPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("DrugsDatabaseSelectorPage"); }

DrugsDatabaseSelectorPage::~DrugsDatabaseSelectorPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString DrugsDatabaseSelectorPage::id() const { return objectName(); }
QString DrugsDatabaseSelectorPage::name() const { return tr("Database selector"); }
QString DrugsDatabaseSelectorPage::category() const { return tkTr(Trans::Constants::DRUGS); }
QString DrugsDatabaseSelectorPage::title() const {return tr("Drug database selector");}
int DrugsDatabaseSelectorPage::sortIndex() const {return 20;}

void DrugsDatabaseSelectorPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void DrugsDatabaseSelectorPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}
void DrugsDatabaseSelectorPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(DrugsDB::Constants::S_DATABASE_PATHS, QVariant());
    defaultvalues.insert(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, QString(DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT));

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k).isNull())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void DrugsDatabaseSelectorPage::finish() { delete m_Widget; }
QString DrugsDatabaseSelectorPage::helpPage()
{
    QString l = QLocale().name().left(2);
    if (l=="fr")
        return Constants::H_PREFERENCES_DBSELECTOR_FR;
    return Constants::H_PREFERENCES_DBSELECTOR_EN;
}

QWidget *DrugsDatabaseSelectorPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new DatabaseSelectorWidget(parent);
    return m_Widget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  DatabaseSelectorWidgetPrivate  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
namespace DrugsWidget {
namespace Internal {
class DatabaseSelectorWidgetPrivate
{
public:
    DatabaseSelectorWidgetPrivate() {}

    ~DatabaseSelectorWidgetPrivate()
    {
        // m_Infos must not be deleted
    }

    QVector<DrugsDB::DatabaseInfos *> m_Infos;
    QString m_SelectedDatabaseUid;
};
}  // End namespace Internal
}  // End namespace DrugsWidget



DatabaseSelectorWidget::DatabaseSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseSelectorWidget),
    d(0)
{
    setObjectName("DatabaseSelectorWidget");
    d = new Internal::DatabaseSelectorWidgetPrivate;
    d->m_SelectedDatabaseUid = settings()->value(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME).toString();

    ui->setupUi(this);
    connect(ui->databaseList, SIGNAL(currentRowChanged(int)), this, SLOT(updateDatabaseInfos(int)));
    setDataToUi();
}

DatabaseSelectorWidget::~DatabaseSelectorWidget()
{
    delete ui;
    ui = 0;
    if (d)
        delete d;
    d = 0;
}

void DatabaseSelectorWidget::setDataToUi()
{
    ui->databaseList->clear();
    d->m_Infos = drugsBase().getAllDrugSourceInformation();
    const DrugsDB::DatabaseInfos *actual = drugsBase().actualDatabaseInformation();
    if (!actual)
        return;
    int row = 0;
    foreach(DrugsDB::DatabaseInfos *info, d->m_Infos) {
        if (!info)
            continue;
        ui->databaseList->addItem(info->translatedName());
        if (info->identifier == actual->identifier) {
            ui->databaseList->setCurrentRow(row, QItemSelectionModel::Select);
        }
        ++row;
    }
}

void DatabaseSelectorWidget::updateDatabaseInfos(int row)
{
    if (row < 0)
        return;

    if (d->m_Infos.isEmpty())
        return;

    if (row >= d->m_Infos.count())
        return;
    d->m_Infos.at(row)->toTreeWidget(ui->infoTree);
    d->m_SelectedDatabaseUid = d->m_Infos.at(row)->identifier;
}

static void changeDrugsDatabase(Core::ISettings *set, const QString &drugBaseUid)
{
    if (!DrugsDB::DrugsModel::activeModel()) {
        set->setValue(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, drugBaseUid);
        drugsBase().refreshDrugsBase();
        return;
    }

    if (set->value(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME).toString() != drugBaseUid) {
        if (DrugsDB::DrugsModel::activeModel()->rowCount()) {
            bool yes = Utils::yesNoMessageBox(
                    QCoreApplication::translate("DatabaseSelectorWidget", "Reset actual prescription"),
                QCoreApplication::translate("DatabaseSelectorWidget", "You have selected a different drugs database than the currently-opened one. "
                   "Your actual prescription will be reset. Do you want to continue?"),
                "", QCoreApplication::translate("DatabaseSelectorWidget", "Drugs database selection"));
            if (!yes) {
                return;
            }
            DrugsDB::DrugsModel::activeModel()->clearDrugsList();
        }
        set->setValue(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, drugBaseUid);
        drugsBase().refreshDrugsBase();
    }
}

void DatabaseSelectorWidget::writeDefaultSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
    LOG_FOR("DatabaseSelectorWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DatabaseSelectorWidget"));
    set->setValue(DrugsDB::Constants::S_DATABASE_PATHS, QVariant());
    changeDrugsDatabase(set, DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT);
}

void DatabaseSelectorWidget::saveToSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }

    // 0. if selected DB is the default one --> change selected to RESOURCE_TAG
//    QString tmp = d->m_SelectedDatabaseUid;
//    QString defaultDbFileName = set->databasePath() + QDir::separator() + QString(DrugsDB::Constants::DB_DRUGS_NAME) + QDir::separator() + QString(DrugsDB::Constants::DB_DRUGS_NAME) + ".db";
//    if (tmp == defaultDbFileName)
//        tmp = DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT;

    // 1. manage application resource path
//    if (tmp.startsWith(settings()->path(Core::ISettings::ReadOnlyDatabasesPath))) {
//        tmp.replace(settings()->path(Core::ISettings::ReadOnlyDatabasesPath), Core::Constants::TAG_APPLICATION_RESOURCES_PATH);
//    }

    // 2. check if user changes the database
    changeDrugsDatabase(set, d->m_SelectedDatabaseUid);
}

void DatabaseSelectorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
