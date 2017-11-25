/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class ZipCodes::GenericZipCodesWidget
 * Ui for user actions for the ZipCodes::GenericZipCodesStep.
*/

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
    ui->downloadButton->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    ui->progressBar->setEnabled(false);
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

    // Connect step signals
//    connect(m_Step, SIGNAL(countryListDownloaded(bool)), ui->localeDataGroupBox, SLOT(setEnabled(bool)));
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(onDownloadFinished()));
    connect(m_Step, SIGNAL(postDownloadProcessingFinished()), this, SLOT(onPostDownloadProcessFinished()));
    connect(m_Step, SIGNAL(processFinished()), this, SLOT(onProcessFinished()));

    // the progressbar must only be connected ONCE here. The m_Step is a member, so the connection
    // will be kept until the step object is deleted = everything is finished.
    connect(m_Step, SIGNAL(progressRangeChanged(int,int)), ui->progressBar, SLOT(setRange(int,int)));
    connect(m_Step, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

    // actions on item activation
    connect(ui->availableCountriesListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(selectCountry(QModelIndex)));
    connect(ui->availableCountriesListView, SIGNAL(clicked(QModelIndex)),
            ui->selectedCountriesListView, SLOT(clearSelection()));
    connect(ui->toolButtonAddCountry, SIGNAL(clicked()), this, SLOT(selectCurrentCountry()));

    connect(ui->selectedCountriesListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(deselectCountry(QModelIndex)));
    connect(ui->selectedCountriesListView, SIGNAL(clicked(QModelIndex)),
            ui->availableCountriesListView, SLOT(clearSelection()));
    connect(ui->toolButtonRemoveCountry, SIGNAL(clicked()), this, SLOT(deselectCurrentCountry()));

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

/*! auto-connected slot, starts downloading the data */
void GenericZipCodesWidget::on_downloadButton_clicked()
{
    if (m_Step->startDownload()) {
        ui->statusLabel->setText(tr("Download in progress"));
        ui->downloadButton->setEnabled(false);
        ui->progressBar->setEnabled(true);
        m_availableCountriesModel->clear();
    }
}

/**
 * Starts the whole datapack creation (one per country + multi-country)
 * \sa ZipCodes::GenericZipCodesStep::populateDatabase()
 */
void GenericZipCodesWidget::on_populateDbButton_clicked()
{
    ui->progressBar->setEnabled(true);
    m_Step->populateDatabase();
    ui->progressBar->setEnabled(false);
    ui->progressBar->setValue(0);
}

/*!
 * Auto-connected slot, populates the available countries model/view
 * using the ZipCodes::GenericZipCodesStep::populateCountryModel(), and
 * select the OS country.
*/
void GenericZipCodesWidget::on_readCountries_clicked()
{
    m_Step->populateCountryModel();
    // find default system country and add it to the selected list
    QList<QStandardItem*> f = m_Step->availableCountriesModel()->findItems(QLocale::countryToString(QLocale::system().country()));
    if (f.count() > 0)
        selectCountry(f.first()->index());
}

/** When download is finished, unzip file */
void GenericZipCodesWidget::onDownloadFinished()
{
    ui->statusLabel->setText(tr("Unzipping downloaded file..."));
    m_Step->postDownloadProcessing();
}

/** When download is finished and post-dowload steps are done */
void GenericZipCodesWidget::onPostDownloadProcessFinished()
{
    ui->statusLabel->setText(tr("File downloaded and unzipped."));
    ui->downloadButton->setIcon(theme()->icon(Core::Constants::ICONSOFTWAREUPDATEAVAILABLE));
    ui->downloadButton->setEnabled(true);
    ui->progressBar->setEnabled(false);
    ui->progressBar->setValue(100);
    m_availableCountriesModel->sort(0);
    ui->createPackageButton->setEnabled(m_selectedCountriesModel->rowCount() > 0);
}

/**
 * Add a country to the selected view/model.
 * This view/model will be used to create a datapack
 * including all the countries included in this model/view
 */
void GenericZipCodesWidget::selectCountry(const QModelIndex &index)
{
    // TODO: code here
    Q_UNUSED(index);
//    const bool validIndex = index.isValid();
//    if (validIndex)
//        m_Step->selectCountry(index);
//    updateActions();
//    ui->createPackageButton->setEnabled(m_selectedCountriesModel->rowCount() > 0);
}

void GenericZipCodesWidget::selectCurrentCountry()
{
    selectCountry(m_availableCountriesModel->index(ui->availableCountriesListView->currentIndex().row(), 0));
}

void GenericZipCodesWidget::deselectCountry(const QModelIndex &index)
{
    // TODO: code here
    Q_UNUSED(index);
//    m_Step->deselectCountry(index);
//    updateActions();
//    ui->createPackageButton->setEnabled(m_selectedCountriesModel->rowCount() > 0);
}

void GenericZipCodesWidget::deselectCurrentCountry()
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

/**
 * Creates all the datapacks files (description + zipped database).\n
 * You can then create or update the datapack server from the datapack page.
 * \sa ZipCodes::GenericZipCodesStep::process()
 */
void GenericZipCodesWidget::on_createPackageButton_clicked()
{
    ui->progressBar->setEnabled(true);
    m_Step->process();
    ui->progressBar->setEnabled(false);
}

void GenericZipCodesWidget::onProcessFinished()
{
}
