/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDb::Internal::DrugsDbModeWidget
 * Allow users to interact with IDrugDatabase objects (download sources, prepare database,
 * populate database...). All IDrugDatabase objects must be registered using the DrugsDbMode object.
 */

#include "drugsdbmodewidget.h"
#include "idrugdatabase.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QProgressDialog>
#include <QPointer>
#include <QVector>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

#include <QDebug>

#include "ui_drugsdbmodewidget.h"

using namespace DrugsDb;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
//static inline DrugsDB::DrugsDBCore *drugsDbCore() {return DrugsDB::DrugsDBCore::instance();}
//static inline DrugsDB::DrugDrugInteractionCore *ddiCore() {return drugsDbCore()->ddiCore();}

namespace DrugsDb {
namespace Internal {

class DrugsDbModeWidgetPrivate
{
public:
    DrugsDbModeWidgetPrivate(DrugsDbModeWidget *parent) :
        ui(0),
        _progress(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~DrugsDbModeWidgetPrivate()
    {
    }

    void updateAvailableDatabase()
    {
        // Update available database listWidget
        QObject::disconnect(ui->availableListWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), q, SLOT(onCurrentDrugsDatabaseChanged(QItemSelection,QItemSelection)));
        ui->availableListWidget->clear();
        foreach(IDrugDatabase *db, _databases) {
            ui->availableListWidget->addItem(db->displayName());
        }
        ui->availableListWidget->setCurrentIndex(QModelIndex());
        QObject::connect(ui->availableListWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), q, SLOT(onCurrentDrugsDatabaseChanged(QItemSelection,QItemSelection)));
    }

    // Returns zero in case of error
    IDrugDatabase *currentDatabase()
    {
        int row = ui->availableListWidget->selectionModel()->currentIndex().row();
        if (!IN_RANGE_STRICT_MAX(row, 0, _databases.count()))
            return 0;
        return _databases.at(row);
    }

    // Set UI 'modifiers' enabled state
    void setUiEnabled(bool enabled)
    {
        ui->freeGroup->setEnabled(enabled);
        ui->nonFreeGroup->setEnabled(enabled);
        ui->download->setEnabled(enabled);
        ui->startJobs->setEnabled(enabled);
        if (!enabled)
            ui->title->setText(QCoreApplication::translate("DrugsDbModeWidget", "Select the drug database to create in the view"));
    }

public:
    Ui::DrugsDbModeWidget *ui;
    QList<IDrugDatabase*> _databases;
    QPointer<QProgressDialog> _progress;

private:
    DrugsDbModeWidget *q;
};
}  // namespace Internal
} // end namespace DrugsDb

/*! Constructor of the DrugsDb::Internal::DrugsDbModeWidget class */
DrugsDbModeWidget::DrugsDbModeWidget(QWidget *parent) :
    QWidget(parent),
    d(new DrugsDbModeWidgetPrivate(this))
{
    d->ui = new Ui::DrugsDbModeWidget();
    d->ui->setupUi(this);
    d->ui->progressBar->hide();

    d->ui->downloadSPC->hide();
    // TODO: connect(d->ui->downloadSPC, SIGNAL(clicked()), this, SLOT(downloadFinished()));

    connect(d->ui->seeDbDescription, SIGNAL(clicked()), this, SLOT(onSeeDatabaseDescriptionFileRequested()));
    connect(d->ui->seeDatapackDescription, SIGNAL(clicked()), this, SLOT(onSeeDatapackDescriptionFileRequested()));

    onCurrentDrugsDatabaseChanged(QItemSelection(), QItemSelection());
}

/*! Destructor of the DrugsDb::Internal::DrugsDbModeWidget class */
DrugsDbModeWidget::~DrugsDbModeWidget()
{
    delete d->ui;
    if (d)
        delete d;
    d = 0;
}

void DrugsDbModeWidget::registerDrugDatabase(IDrugDatabase *drugDatabase)
{
    d->_databases << drugDatabase;
    d->updateAvailableDatabase();
}

void DrugsDbModeWidget::on_startJobs_clicked()
{
    // Wrong db index, no index?
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;

    if (d->_progress)
        delete d->_progress;

    // Prepare the progress dialog
    d->_progress = new QProgressDialog(mainwindow());
    d->_progress->setLabelText(tr("Starting jobs"));
    d->_progress->setRange(0, 1);
    d->_progress->setWindowModality(Qt::WindowModal);
    d->_progress->setValue(0);
    d->_progress->show();

    // Get the IDrugDatabase to work with
    connect(base, SIGNAL(progressRangeChanged(int,int)), this, SLOT(changeStepProgressRange(int,int)));
    connect(base, SIGNAL(progress(int)), d->_progress, SLOT(setValue(int)));
    connect(base, SIGNAL(progressLabelChanged(QString)), d->_progress, SLOT(setLabelText(QString)));

    if (d->ui->unzip->isChecked() && !d->ui->unzip->text().contains("CORRECTLY DONE")) {
        if (base->unzipFiles())
            d->ui->unzip->setText(d->ui->unzip->text() + " CORRECTLY DONE");
    }
    if (d->ui->prepare->isChecked() && !d->ui->prepare->text().contains("CORRECTLY DONE")) {
        if (base->prepareData())
            d->ui->prepare->setText(d->ui->prepare->text() + " CORRECTLY DONE");
    }
    if (d->ui->createDb->isChecked() && !d->ui->createDb->text().contains("CORRECTLY DONE")) {
        if (base->createDatabase())
            d->ui->createDb->setText(d->ui->createDb->text() + " CORRECTLY DONE");
    }
    if (d->ui->populate->isChecked() && !d->ui->populate->text().contains("CORRECTLY DONE")) {
        if (base->populateDatabase())
            d->ui->populate->setText(d->ui->populate->text() + " CORRECTLY DONE");
    }

    // Non-free part
    if (!base->createDatabase()) {
        LOG_ERROR("Unable to create/connect drug database");
        d->_progress->setRange(0, 1);
        d->_progress->setValue(1);
        return;
    }
    if (d->ui->addAtc->isChecked() && !d->ui->addAtc->text().contains("CORRECTLY DONE")) {
        if (base->addAtc())
            d->ui->addAtc->setText(d->ui->addAtc->text() + " CORRECTLY DONE");
    }
    if (d->ui->linkMols->isChecked() && !d->ui->linkMols->text().contains("CORRECTLY DONE")) {
        if (base->linkDrugsComponentsAndDrugInteractors())
            d->ui->linkMols->setText(d->ui->linkMols->text() + " CORRECTLY DONE");
    }
    if (d->ui->addDDI->isChecked() && !d->ui->addDDI->text().contains("CORRECTLY DONE")) {
        if (base->addDrugDrugInteractions())
            d->ui->addDDI->setText(d->ui->addDDI->text() + " CORRECTLY DONE");
    }
    if (d->ui->addPims->isChecked() && !d->ui->addPims->text().contains("CORRECTLY DONE")) {
        if (base->addPims())
            d->ui->addPims->setText(d->ui->addPims->text() + " CORRECTLY DONE");
    }
    if (d->ui->addPreg->isChecked() && !d->ui->addPreg->text().contains("CORRECTLY DONE")) {
        if (base->addPregnancyCheckingData())
            d->ui->addPreg->setText(d->ui->addPreg->text() + " CORRECTLY DONE");
    }
    if (d->ui->spc->isChecked() && !d->ui->spc->text().contains("CORRECTLY DONE")) {
        if (base->downloadSpcContents())
            d->ui->spc->setText(d->ui->spc->text() + " CORRECTLY DONE");
    }
    Utils::Log::messagesToTreeWidget(d->ui->messages);
    Utils::Log::errorsToTreeWidget(d->ui->errors);

    d->_progress->setRange(0, 1);
    d->_progress->setValue(1);
}

bool DrugsDbModeWidget::on_download_clicked()
{
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return false;

    d->ui->download->setEnabled(false);
    d->ui->progressBar->show();
    connect(base, SIGNAL(progressRangeChanged(int,int)), d->ui->progressBar, SLOT(setRange(int,int)));
    connect(base, SIGNAL(progress(int)), d->ui->progressBar, SLOT(setValue(int)));
    connect(base, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    base->startDownload();
    return true;
}

void DrugsDbModeWidget::downloadFinished()
{
    d->ui->progressBar->hide();
    d->ui->download->setEnabled(true);
}

void DrugsDbModeWidget::changeStepProgressRange(int min, int max)
{
    // IDrugDatabase *base = qobject_cast<IDrugDatabase*>(sender());
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;
    if (base && d->_progress)
        d->_progress->setRange(min, max);
}

/** When user select a new current database, update the information */
void DrugsDbModeWidget::onCurrentDrugsDatabaseChanged(const QItemSelection &current, const QItemSelection &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    d->setUiEnabled(false);
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;
    d->setUiEnabled(true);

    // Update UI labels and texts
    d->ui->output->setText(base->outputPath() + QDir::separator() + base->outputFileName());
    d->ui->title->setText(base->displayName());

    // Add tooltips to buttons
    d->ui->seeDbDescription->setToolTip(base->databaseDescriptionFile());
    d->ui->seeDatapackDescription->setToolTip(base->datapackDescriptionFile());
}

void DrugsDbModeWidget::onSeeDatabaseDescriptionFileRequested()
{
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;
    // TODO: open file in a TextEditor that allow save & edit
    QDesktopServices::openUrl(QUrl(base->databaseDescriptionFile()));
}

void DrugsDbModeWidget::onSeeDatapackDescriptionFileRequested()
{
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;
    // TODO: open file in a TextEditor that allow save & edit
    QDesktopServices::openUrl(QUrl(base->datapackDescriptionFile()));
}

//void DrugsDbModeWidget::showEvent(QShowEvent *event)
//{
//    IDrugDatabase *base = d->currentDatabase();
//    if (!base)
//        return;
//    if (base->licenseType() == IDrugDatabase::Free) {
//        d->ui->addAtc->setChecked(false);
//        d->ui->linkMols->setChecked(false);
//        d->ui->addDDI->setChecked(false);
//        d->ui->addPims->setChecked(false);
//        d->ui->addPreg->setChecked(false);

//        d->ui->addAtc->setEnabled(false);
//        d->ui->linkMols->setEnabled(false);
//        d->ui->addDDI->setEnabled(false);
//        d->ui->addPims->setEnabled(false);
//        d->ui->addPreg->setEnabled(false);
//        QWidget::showEvent(event);
//        return;
//    }


//    // check the possibilities of the ddiCore
////    bool atc = ddiCore()->canAddAtc();
////    if (!atc) {
////        d->ui->addAtc->setChecked(false);
////        d->ui->linkMols->setChecked(false);
////    }
////    d->ui->addAtc->setEnabled(atc);
////    d->ui->linkMols->setEnabled(atc);

////    bool ddi = ddiCore()->canAddDrugDrugInteractions();
////    if (!ddi)
////        d->ui->addDDI->setChecked(false);
////    d->ui->addDDI->setEnabled(ddi);

////    bool pim = ddiCore()->canAddPims();
////    if (!pim)
////        d->ui->addPims->setChecked(false);
////    d->ui->addPims->setEnabled(pim);

////    bool preg = ddiCore()->canAddPregnancyChecking();
////    if (!preg)
////        d->ui->addPreg->setChecked(false);
////    d->ui->addPreg->setEnabled(preg);
//    QWidget::showEvent(event);
//}
