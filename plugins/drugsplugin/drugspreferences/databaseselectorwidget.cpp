/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "databaseselectorwidget.h"
#include "ui_databaseselectorwidget.h"
#include "constants.h"

#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

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


/** \todo How to manage drugs base changing into FreeMedForms ? */

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

void DrugsDatabaseSelectorPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
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
    DatabaseSelectorWidgetPrivate() : m_PathModel(0) {}

    ~DatabaseSelectorWidgetPrivate()
    {
        // m_Infos must not be deleted
    }

    QVector<DrugsDB::DatabaseInfos *> m_Infos;
    QStringListModel *m_PathModel;
    QString m_SelectedDatabaseFileName;
};
}  // End namespace Internal
}  // End namespace DrugsWidget



DatabaseSelectorWidget::DatabaseSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseSelectorWidget),
    d(0)
{
    d = new Internal::DatabaseSelectorWidgetPrivate;
    d->m_SelectedDatabaseFileName = settings()->value(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME).toString();
    if (d->m_SelectedDatabaseFileName.startsWith(Core::Constants::TAG_APPLICATION_RESOURCES_PATH)) {
        d->m_SelectedDatabaseFileName.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::ReadOnlyDatabasesPath));
    }

    ui->setupUi(this);
    ui->pathView->hide();
    ui->addPathButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    ui->removePathButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    ui->tooglePathsButton->setIcon(theme()->icon(Core::Constants::ICONEYES));

    d->m_PathModel = new QStringListModel(this);
    d->m_PathModel->setStringList(settings()->value(DrugsDB::Constants::S_DATABASE_PATHS).toStringList());
    ui->pathView->setModel(d->m_PathModel);

    connect(ui->databaseList, SIGNAL(currentRowChanged(int)), this, SLOT(updateDatabaseInfos(int)));
    connect(ui->addPathButton, SIGNAL(clicked()), this, SLOT(addPath()));
    connect(ui->removePathButton, SIGNAL(clicked()), this, SLOT(removePath()));
    connect(ui->tooglePathsButton, SIGNAL(clicked()), this, SLOT(tooglePaths()));

    connect(d->m_PathModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(getAllAvailableDatabases()));

    setDatasToUi();
}

DatabaseSelectorWidget::~DatabaseSelectorWidget()
{
    delete ui;
    ui = 0;
    if (d)
        delete d;
    d = 0;
}

void DatabaseSelectorWidget::setDatasToUi()
{
    ui->databaseList->clear();
    if (!selector())
        return;
    selector()->getAllDatabaseInformations(d->m_PathModel->stringList());
    d->m_Infos = selector()->availableDatabases();
    const DrugsDB::DatabaseInfos *actual = DrugsDB::Internal::DrugsBase::instance()->actualDatabaseInformations();
    int row = 0;
    foreach(DrugsDB::DatabaseInfos *info, d->m_Infos) {
        ui->databaseList->addItem(info->translatedName());
        if (!actual)
            continue;
        if (info->fileName == actual->fileName) {
            ui->databaseList->setCurrentRow(row, QItemSelectionModel::Select);
        }
        ++row;
    }
}

void DatabaseSelectorWidget::getAllAvailableDatabases()
{
    if (!selector())
        return;
    selector()->getAllDatabaseInformations(d->m_PathModel->stringList());
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
    d->m_SelectedDatabaseFileName = d->m_Infos.at(row)->fileName;
}

void DatabaseSelectorWidget::addPath()
{
    QString dir = QFileDialog::getExistingDirectory(qApp->activeWindow(), tr("Select directory"),
                                                    QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    if (d->m_PathModel->stringList().contains(dir))
        return;
    int row = d->m_PathModel->rowCount();
    d->m_PathModel->insertRow(row);
    d->m_PathModel->setData(d->m_PathModel->index(row,0), dir);
    setDatasToUi();
}

void DatabaseSelectorWidget::removePath()
{
    if (!ui->pathView->selectionModel()->hasSelection())
        return;
    d->m_PathModel->removeRow(ui->pathView->selectionModel()->currentIndex().row());
    setDatasToUi();
}

void DatabaseSelectorWidget::tooglePaths()
{
    ui->pathView->setVisible(!ui->pathView->isVisible());
}

static void changeDrugsDatabase(Core::ISettings *set, const QString &fileName)
{
    if (!DrugsDB::DrugsModel::activeModel()) {
        set->setValue(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, fileName);
        return;
    }

    if (set->value(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME).toString() != fileName) {
        if (DrugsDB::DrugsModel::activeModel()->rowCount()) {
            bool yes = Utils::yesNoMessageBox(
                    QCoreApplication::translate("DatabaseSelectorWidget", "Reset actual prescription"),
                QCoreApplication::translate("DatabaseSelectorWidget", "You have selected a different drugs database than the currently-opened one. "
                   "Your actual prescription will be resetted. Do you want to continue ?"),
                "", QCoreApplication::translate("DatabaseSelectorWidget", "Drugs database selection"));
            if (!yes) {
                return;
            }
            DrugsDB::DrugsModel::activeModel()->clearDrugsList();
        }
        set->setValue(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, fileName);
        DrugsDB::Internal::DrugsBase::instance()->refreshDrugsBase();
    }
}

void DatabaseSelectorWidget::writeDefaultSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
    Utils::Log::addMessage("DatabaseSelectorWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DatabaseSelectorWidget"));
    set->setValue(DrugsDB::Constants::S_DATABASE_PATHS, QVariant());
    changeDrugsDatabase(set, DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT);
    set->sync();
}

void DatabaseSelectorWidget::saveToSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
    set->setValue(DrugsDB::Constants::S_DATABASE_PATHS, d->m_PathModel->stringList());

    // 0. if selected DB is the default one --> change selected to RESOURCE_TAG
    QString tmp = d->m_SelectedDatabaseFileName;
    QString defaultDbFileName = set->databasePath() + QDir::separator() + QString(DrugsDB::Constants::DB_DRUGS_NAME) + QDir::separator() + QString(DrugsDB::Constants::DB_DRUGS_NAME) + ".db";
    if (tmp == defaultDbFileName)
        tmp = DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT;

    // 1. manage application resource path
    if (tmp.startsWith(settings()->path(Core::ISettings::ReadOnlyDatabasesPath))) {
        tmp.replace(settings()->path(Core::ISettings::ReadOnlyDatabasesPath), Core::Constants::TAG_APPLICATION_RESOURCES_PATH);
    }

    // 2. check if user changes the database
    changeDrugsDatabase(set, tmp);
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
