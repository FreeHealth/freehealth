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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Icd10::Internal::Icd10DatabasePage
 * Option page for the Free ICD10 database.
 */

#include "icd10databasecreator.h"
#include "icd10step.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QDebug>

#include "ui_icd10databasewidget.h"

using namespace Icd10;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

/**
 * The ctor also create the Core::IFullReleaseStep object and
 * register it in the plugin manager object pool.
 */
Icd10DatabasePage::Icd10DatabasePage(QObject *parent) :
    IToolPage(parent),
    _step(0)
{
    setObjectName("Icd10DatabasePage_IToolPage");
    _step = new Icd10Step(this);
    pluginManager()->addObject(_step);
}

Icd10DatabasePage::~Icd10DatabasePage()
{
    pluginManager()->removeObject(_step);
}

QString Icd10DatabasePage::name() const
{return tr("ICD10 Database Creator");}

QString Icd10DatabasePage::category() const
{return Core::Constants::CATEGORY_ICD10DATABASE;}

QIcon Icd10DatabasePage::icon() const
{return QIcon();}

/** Create the option page */
QWidget *Icd10DatabasePage::createPage(QWidget *parent)
{
    Icd10DatabaseWidget *widget = new Icd10DatabaseWidget(parent);
    widget->setStep(_step);
    return widget;
}

/** Widget for the Icd10::Internal::Icd10DatabasePage */
Icd10DatabaseWidget::Icd10DatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Icd10DatabaseWidget),
    _step(0)
{
    setObjectName("Icd10DatabaseWidget");
    ui->setupUi(this);
}

Icd10DatabaseWidget::~Icd10DatabaseWidget()
{
    delete ui;
}

/**
 * Define the step to use with the UI
 * \sa Icd10::Internal::Icd10DatabasePage
*/
void Icd10DatabaseWidget::setStep(Icd10::Internal::Icd10Step *step)
{
    _step = step;
    connect(_step, SIGNAL(downloadFinished()), this, SLOT(onDownloadFinished()), Qt::UniqueConnection);
}

/**
 * Start the creation of the ICD10 database and datapack registration.
 * \sa Icd10::Internal::Icd10Step
*/
bool Icd10DatabaseWidget::on_startCreation_clicked()
{
    Q_ASSERT(_step);
    if (!_step)
        return false;
    if (!_step->createTemporaryStorage())
        return false;
    if (!_step->startDownload())
        return false;
    return true;
}

/**
 * Continue the creation of the ICD10 database and datapack registration.
 * \sa Icd10::Internal::Icd10Step
*/
bool Icd10DatabaseWidget::onDownloadFinished()
{
    Q_ASSERT(_step);
    if (!_step)
        return false;
    if (!_step->postDownloadProcessing())
        return false;
    if (!_step->process())
        return false;
    if (!_step->registerDataPack())
        return false;
    return true;
}

void Icd10DatabaseWidget::changeEvent(QEvent *e)
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

