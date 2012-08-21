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

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/isettings.h>

using namespace ZipCodes;

namespace {
static inline ExtensionSystem::PluginManager* pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ITheme* theme() { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
}

GenericZipCodesWidget::GenericZipCodesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenericZipCodesWidget)
{
    ui->setupUi(this);
    ui->downloadButton->setIcon(theme()->icon(Core::Constants::ICONSOFTWAREUPDATEAVAILABLE));
    ui->availCountriesCombo->setFlagPath(settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/");

    m_Step = new GenericZipCodesStep(this);
    m_Step->setCountryCombo(ui->availCountriesCombo);
    m_Step->createDir();
    pluginManager()->addObject(m_Step);

    // after downloading the countries list, enable/disable the widgets according to download success
    connect(m_Step, SIGNAL(countryListDownloaded(bool)), ui->localeDataGroupBox, SLOT(setEnabled(bool)));
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
//    ui->progressBar->setValue(100);
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

