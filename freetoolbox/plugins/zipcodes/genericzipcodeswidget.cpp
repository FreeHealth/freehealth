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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "genericzipcodeswidget.h"
#include "ui_genericzipcodeswidget.h"

#include <utils/global.h>
#include <extensionsystem/pluginmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/isettings.h>

#include <QStandardItemModel>

using namespace ZipCodes;

namespace {
static inline ExtensionSystem::PluginManager* pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ITheme* theme() { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
const QString flagPath = settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/";
}

GenericZipCodesWidget::GenericZipCodesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenericZipCodesWidget)
{
    ui->setupUi(this);
    ui->downloadButton->setIcon(theme()->icon(Core::Constants::ICONSOFTWAREUPDATEAVAILABLE));
    ui->toolButtonAddCountry->setIcon(theme()->icon(Core::Constants::ICONADD));
    ui->toolButtonRemoveCountry->setIcon(theme()->icon(Core::Constants::ICONREMOVE));

    m_Step = new GenericZipCodesStep(this);
    m_Step->createTemporaryStorage();
    pluginManager()->addObject(m_Step);

    // provide convenience pointers for fast access to the model data of the step object
    m_availableCountriesModel = m_Step->availableCountriesModel();
    ui->availableCountriesListView->setModel(m_availableCountriesModel);

    m_selectedCountriesModel = m_Step->selectedCountriesModel();
    ui->selectedCountriesListView->setModel(m_selectedCountriesModel);

    // after downloading the countries list, enable/disable the widgets according to download success
    connect(m_Step, SIGNAL(countryListDownloaded(bool)),
            ui->localeDataGroupBox, SLOT(setEnabled(bool)));

    // actions on item activation
    connect(ui->availableCountriesListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(selectCountry(QModelIndex)));
    connect(ui->availableCountriesListView, SIGNAL(clicked(QModelIndex)),
            ui->selectedCountriesListView, SLOT(clearSelection()));
    connect(ui->toolButtonAddCountry, SIGNAL(clicked()), this, SLOT(selectCountry()));

    connect(ui->selectedCountriesListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(deselectCountry(QModelIndex)));
    connect(ui->selectedCountriesListView, SIGNAL(clicked(QModelIndex)),
            ui->availableCountriesListView, SLOT(clearSelection()));
    connect(ui->toolButtonRemoveCountry, SIGNAL(clicked()), this, SLOT(deselectCountry()));

    // update actions after selection changes
    connect(ui->availableCountriesListView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateActions()));
    connect(ui->selectedCountriesListView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateActions()));
    updateActions();
}

GenericZipCodesWidget::~GenericZipCodesWidget()
{
    pluginManager()->removeObject(m_Step);
    delete ui;
}

/*! auto-connected slot, starts querying the webservice for getting the countries list */
void GenericZipCodesWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setEnabled(false);
//    ui->progressBar->setEnabled(true);
//    ui->downloadButton->setText(tr("Download in progress"));
    m_availableCountriesModel->clear();
    connect(m_Step, SIGNAL(countryListDownloaded(bool)), this, SLOT(onDownloadFinished()));
    m_Step->startDownload();
}

void GenericZipCodesWidget::on_createPackageButton_clicked()
{
    m_Step->process();
    connect(m_Step, SIGNAL(processFinished()), this, SLOT(onProcessFinished()));
}

void GenericZipCodesWidget::onDownloadFinished()
{
//    ui->downloadButton->setText(tr("Download terminated"));
    ui->downloadButton->setEnabled(true);
    m_availableCountriesModel->sort(0);
    //    ui->progressBar->setValue(100);
    ui->createPackageButton->setEnabled(m_selectedCountriesModel->rowCount() > 0);
}

void GenericZipCodesWidget::onProcessFinished()
{
    m_Step->postDownloadProcessing();
}

void GenericZipCodesWidget::selectCountry(const QModelIndex &index)
{
    const bool validIndex = index.isValid();
    if (validIndex)
        m_Step->selectCountry(index);
    updateActions();
    ui->createPackageButton->setEnabled(m_selectedCountriesModel->rowCount() > 0);
}

void GenericZipCodesWidget::selectCountry()
{
    selectCountry(m_availableCountriesModel->index(ui->availableCountriesListView->currentIndex().row(), 0));
}

void GenericZipCodesWidget::deselectCountry(const QModelIndex &index)
{
    m_Step->deselectCountry(index);
    updateActions();
    ui->createPackageButton->setEnabled(m_selectedCountriesModel->rowCount() > 0);
}

void GenericZipCodesWidget::deselectCountry()
{
    deselectCountry(m_selectedCountriesModel->index(ui->selectedCountriesListView->currentIndex().row(), 0));
}

void GenericZipCodesWidget::updateActions()
{
    QItemSelectionModel *model = ui->availableCountriesListView->selectionModel();
    ui->toolButtonAddCountry->setEnabled(model && model->hasSelection());

    model = ui->selectedCountriesListView->selectionModel();
    ui->toolButtonRemoveCountry->setEnabled(model && model->hasSelection());
}
