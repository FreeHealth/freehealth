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

    m_Step = new GenericZipCodesStep(this);
    QStandardItemModel *availableCountriesModel = new QStandardItemModel(this);
    m_Step->setAvailableCountriesWidget(ui->availableCountriesListWidget);

    m_Step->createDir();
    pluginManager()->addObject(m_Step);

    ui->toolButtonAddCountry->setIcon(theme()->icon(Core::Constants::ICONADD));
    ui->toolButtonRemoveCountry->setIcon(theme()->icon(Core::Constants::ICONREMOVE));

    // after downloading the countries list, enable/disable the widgets according to download success
    connect(m_Step, SIGNAL(countryListDownloaded(bool)),
            ui->localeDataGroupBox, SLOT(setEnabled(bool)));

    connect(ui->availableCountriesListWidget, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(selectCountry(QListWidgetItem*)));
    connect(ui->availableCountriesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(updateSelections()));

    connect(ui->selectedCountriesListWidget, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(deselectCountry(QListWidgetItem*)));
    connect(ui->selectedCountriesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(updateSelections()));

    connect(ui->toolButtonAddCountry, SIGNAL(clicked()), this, SLOT(selectCountries()));
    connect(ui->toolButtonRemoveCountry, SIGNAL(clicked()), this, SLOT(deselectCountries()));
    // update actions after selection changes
    connect(ui->availableCountriesListWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(updateActions()));
    connect(ui->selectedCountriesListWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(updateActions()));
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
    m_Step->downloadFiles();
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
}

void GenericZipCodesWidget::downloadFinished()
{
//    ui->downloadButton->setText(tr("Download terminated"));
    ui->downloadButton->setEnabled(true);
    ui->availableCountriesListWidget->sortItems();
    //    ui->progressBar->setValue(100);
}

void GenericZipCodesWidget::selectCountry(QListWidgetItem *item)
{
    if (!item)
        return;
    // takeitem() does not delete *item, so pointer is still available afterwords
    ui->availableCountriesListWidget->takeItem(ui->availableCountriesListWidget->row(item));
    ui->selectedCountriesListWidget->addItem(item);
    ui->selectedCountriesListWidget->sortItems();
    updateActions();
    ui->createPackageButton->setEnabled(true);
}

void GenericZipCodesWidget::selectCountries()
{
    foreach(QListWidgetItem *item, ui->availableCountriesListWidget->selectedItems()) {
        selectCountry(item);
    }
}

void GenericZipCodesWidget::deselectCountry(QListWidgetItem *item)
{
    if (!item)
        return;
    // takeitem() does not delete *item, so pointer is still available afterwords
    ui->selectedCountriesListWidget->takeItem(ui->selectedCountriesListWidget->row(item));
    ui->availableCountriesListWidget->addItem(item);
    ui->availableCountriesListWidget->sortItems();
    updateActions();
    ui->createPackageButton->setEnabled(ui->selectedCountriesListWidget->count() > 0);
}

void GenericZipCodesWidget::deselectCountries()
{
    foreach(QListWidgetItem *item, ui->selectedCountriesListWidget->selectedItems()) {
        deselectCountry(item);
    }
}

void GenericZipCodesWidget::updateActions()
{
    ui->toolButtonAddCountry->setEnabled(!ui->availableCountriesListWidget->selectedItems().isEmpty());
    ui->toolButtonRemoveCountry->setEnabled(!ui->selectedCountriesListWidget->selectedItems().isEmpty());
}

void GenericZipCodesWidget::updateSelections()
{
    if (sender() == ui->availableCountriesListWidget) {
        ui->selectedCountriesListWidget->clearSelection();
    } else if (sender() == ui->selectedCountriesListWidget) {
        ui->availableCountriesListWidget->clearSelection();
    }

}

void GenericZipCodesWidget::on_startJob_clicked()
{
//    m_Step->createDir();
//    m_Step->cleanFiles();
//    m_Step->downloadFiles();
    m_Step->process();

//    m_Step->processMessage();

//    m_Step->postProcessDownload();
}

