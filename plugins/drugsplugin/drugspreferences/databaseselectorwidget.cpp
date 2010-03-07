/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "databaseselectorwidget.h"
#include "ui_databaseselectorwidget.h"

#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsbase.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>

using namespace DrugsWidget;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugsDatabaseSelector *selector() {return DrugsDB::DrugsDatabaseSelector::instance();}


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
    defaultvalues.insert(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, QString(DrugsDB::Constants::DEFAULT_DATABASE_IDENTIFIANT));

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k).isNull())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

void DrugsDatabaseSelectorPage::finish() { delete m_Widget; }

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
    selector()->getAllDatabaseInformations(d->m_PathModel->stringList());
    d->m_Infos = selector()->availableDatabases();
    const DrugsDB::DatabaseInfos *actual = DrugsDB::Internal::DrugsBase::instance()->actualDatabaseInformations();
    int row = 0;
    foreach(DrugsDB::DatabaseInfos *info, d->m_Infos) {
        ui->databaseList->addItem(info->translatedName());
        if (info->fileName == actual->fileName) {
            ui->databaseList->setCurrentRow(row, QItemSelectionModel::Select);
        }
        ++row;
    }
}

void DatabaseSelectorWidget::getAllAvailableDatabases()
{
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

void DatabaseSelectorWidget::writeDefaultSettings(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set) {
        set = settings();
    }
    Utils::Log::addMessage("DatabaseSelectorWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("DatabaseSelectorWidget"));
    set->setValue(DrugsDB::Constants::S_DATABASE_PATHS, QVariant());
    if (set->value(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME).toString() != QString(DrugsDB::Constants::DEFAULT_DATABASE_IDENTIFIANT)) {
        Utils::warningMessageBox(tr("Application must be restarted to take changes into account."),
                                 tr("You have selected a different drugs database than the currently-opened on. "
                                    "You need to restart the application."),
                                 "", tr("Drugs database selection"));
    }
    set->setValue(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, QString(DrugsDB::Constants::DEFAULT_DATABASE_IDENTIFIANT));
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
    QString defaultDbFileName = set->databasePath() + QDir::separator() + QString(DrugsDB::Constants::DRUGS_DATABASE_NAME) + QDir::separator() + QString(DrugsDB::Constants::DRUGS_DATABASE_NAME) + ".db";
    if (tmp == defaultDbFileName)
        tmp = DrugsDB::Constants::DEFAULT_DATABASE_IDENTIFIANT;

    // 1. manage application resource path
    if (tmp.startsWith(settings()->path(Core::ISettings::ReadOnlyDatabasesPath))) {
        tmp.replace(settings()->path(Core::ISettings::ReadOnlyDatabasesPath), Core::Constants::TAG_APPLICATION_RESOURCES_PATH);
    }

    // 2. check if user changes the database
    if (set->value(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME).toString() != tmp) {
        Utils::warningMessageBox(tr("Application must be restarted to take changes into account."),
                                 tr("You have selected a different drugs database than the currently-opened on. "
                                    "You need to restart the application."),
                                 "", tr("Drugs database selection"));
    }
    // 3. save the value
    set->setValue(DrugsDB::Constants::S_SELECTED_DATABASE_FILENAME, tmp);
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
