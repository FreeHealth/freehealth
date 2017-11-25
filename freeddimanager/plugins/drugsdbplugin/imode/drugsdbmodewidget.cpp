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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDb::Internal::DrugsDbModeWidget
 * Allow users to interact with IDrugDatabase objects (download sources,
 * prepare database, populate database...). All IDrugDatabase objects
 * must be registered using the DrugsDbMode object.
 */

#include "drugsdbmodewidget.h"
#include "idrugdatabase.h"

#include <ddiplugin/constants.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/components/componentatcmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/fdm_constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <datapackutils/constants.h>
#include <datapackutils/pack.h>
#include <datapackutils/packdescription.h>
#include <datapackutils/servercreation/packcreationqueue.h>

#include <QProgressDialog>
#include <QPointer>
#include <QVector>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>

#include <QDebug>

#include "ui_drugsdbmodewidget.h"

using namespace DrugsDb;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ModeManager *modeManager() {return Core::ICore::instance()->modeManager();}
static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}

namespace DrugsDb {
namespace Internal {

class DrugsDbModeWidgetPrivate
{
public:
    DrugsDbModeWidgetPrivate(DrugsDbModeWidget *parent) :
        ui(0),
        _progress(0),
        _treeNonFree(0),
        _treeFree(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~DrugsDbModeWidgetPrivate()
    {
    }

    // Get all available database creator objects from the pool
    // and construct the trees (free/nonfree)
    void updateAvailableDatabase()
    {
        // Update available database listWidget
        QObject::disconnect(ui->availableTreeWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), q, SLOT(onCurrentDrugsDatabaseChanged(QItemSelection,QItemSelection)));
        if (!_treeNonFree) {
            _treeNonFree = new QTreeWidgetItem(ui->availableTreeWidget, QStringList() << "Non-Free");
            QFont bold;
            bold.setBold(true);
            _treeNonFree->setFont(0, bold);
        }
        if (!_treeFree) {
            _treeFree = new QTreeWidgetItem(ui->availableTreeWidget, QStringList() << "Free");
            QFont bold;
            bold.setBold(true);
            _treeFree->setFont(0, bold);
        }
        foreach(IDrugDatabase *db, _databases) {
            if (_databaseItems.values().contains(db))
                continue;
            QTreeWidgetItem *item = 0;
            QFont bold;
            bold.setBold(true);
            if (db->licenseType() == IDrugDatabase::NonFree) {
                _treeNonFree->addChild(item);
                // Create a country child
                item = new QTreeWidgetItem(_treeNonFree, QStringList() << QLocale::countryToString(db->country()));
                item->setFont(0, bold);
                item->setForeground(0, QBrush(Qt::darkBlue));
                item = new QTreeWidgetItem(item, QStringList() << db->displayName());
            } else {
                _treeFree->addChild(item);
                item = new QTreeWidgetItem(_treeFree, QStringList() << QLocale::countryToString(db->country()));
                item->setFont(0, bold);
                item->setForeground(0, QBrush(Qt::darkBlue));
                item = new QTreeWidgetItem(item, QStringList() << db->displayName());
            }
            _databaseItems.insert(item, db);
        }
        ui->availableTreeWidget->setCurrentIndex(QModelIndex());
        ui->availableTreeWidget->expandAll();
        QObject::connect(ui->availableTreeWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), q, SLOT(onCurrentDrugsDatabaseChanged(QItemSelection,QItemSelection)));
    }

    // Returns zero in case of error
    IDrugDatabase *currentDatabase()
    {
        return _databaseItems.value(ui->availableTreeWidget->currentItem());
    }

    // Set UI 'modifiers' enabled state
    void setUiEnabled(bool enabled)
    {
        ui->freeGroup->setEnabled(enabled);
        ui->nonFreeGroup->setEnabled(enabled);
        ui->download->setEnabled(enabled);
        ui->startJobs->setEnabled(enabled);
        ui->seeDbDescription->setEnabled(enabled);
        ui->seeDatapackDescription->setEnabled(enabled);
        ui->createDatapack->setEnabled(enabled);
        ui->showDbReport->setEnabled(enabled);

        if (!enabled)
            ui->title->setText(QCoreApplication::translate("DrugsDbModeWidget", "Select the drug database to create in the view"));
    }

public:
    Ui::DrugsDbModeWidget *ui;
    QList<IDrugDatabase*> _databases;
    QPointer<QProgressDialog> _progress;
    QTreeWidgetItem *_treeNonFree, *_treeFree;
    QHash<QTreeWidgetItem *, IDrugDatabase *> _databaseItems;

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
    d->ui->sourceSelector->hide();
    // TODO: connect(d->ui->downloadSPC, SIGNAL(clicked()), this, SLOT(downloadFinished()));

    connect(d->ui->seeDbDescription, SIGNAL(clicked()), this, SLOT(onSeeDatabaseDescriptionFileRequested()));
    connect(d->ui->seeDatapackDescription, SIGNAL(clicked()), this, SLOT(onSeeDatapackDescriptionFileRequested()));
    connect(d->ui->createDatapack, SIGNAL(clicked()), this, SLOT(onCreateDatapackFiles()));
    connect(d->ui->showDbReport, SIGNAL(clicked()), this, SLOT(onShowDatabaseReportRequested()));
    connect(d->ui->seeComponents, SIGNAL(clicked()), this, SLOT(onShowComponentsRequested()));
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

    base->clearFinalReport();

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

    d->_progress->setRange(0, 1);
    d->_progress->setValue(1);

    base->showReportDialog();
}

bool DrugsDbModeWidget::on_download_clicked()
{
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return false;

    base->clearFinalReport();
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

    // Wrong db index, no index?
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;

    // TODO: show a 'job finished' dialog with the content of the finalReport() of the IDrugDatabase
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
    QFileInfo info(QString("%1/%2").arg(base->outputPath()).arg(base->outputFileName()));
    d->ui->fileNameLabel->setText(info.fileName());
    d->ui->path->setText(info.absolutePath());
    d->ui->dateLabel->setText(QLocale().toString(info.lastModified(), QLocale::LongFormat));
    d->ui->sizeLabel->setText(QLocale().toString(info.size()));

    // Reset all checkboxes label
    d->ui->unzip->setText(d->ui->unzip->text().remove(" CORRECTLY DONE"));
    d->ui->prepare->setText(d->ui->prepare->text().remove(" CORRECTLY DONE"));
    d->ui->createDb->setText(d->ui->createDb->text().remove(" CORRECTLY DONE"));
    d->ui->populate->setText(d->ui->populate->text().remove(" CORRECTLY DONE"));
    d->ui->addAtc->setText(d->ui->addAtc->text().remove(" CORRECTLY DONE"));
    d->ui->linkMols->setText(d->ui->linkMols->text().remove(" CORRECTLY DONE"));
    d->ui->addDDI->setText(d->ui->addDDI->text().remove(" CORRECTLY DONE"));
    d->ui->addPims->setText(d->ui->addPims->text().remove(" CORRECTLY DONE"));
    d->ui->addPreg->setText(d->ui->addPreg->text().remove(" CORRECTLY DONE"));
    d->ui->spc->setText(d->ui->spc->text().remove(" CORRECTLY DONE"));

    // Uncheck all checkboxes
    d->ui->unzip->setChecked(false);
    d->ui->prepare->setChecked(false);
    d->ui->createDb->setChecked(false);
    d->ui->populate->setChecked(false);
    d->ui->addAtc->setChecked(false);
    d->ui->linkMols->setChecked(false);
    d->ui->addDDI->setChecked(false);
    d->ui->addPims->setChecked(false);
    d->ui->addPreg->setChecked(false);
    d->ui->spc->setChecked(false);

    // Add tooltips to buttons
    d->ui->seeDbDescription->setToolTip(base->databaseDescriptionFilePath());
    d->ui->seeDatapackDescription->setToolTip(base->datapackDescriptionFilePath());

    // Enable ui parts according to licence type
    bool enableNonFree = (base->licenseType() == IDrugDatabase::NonFree);
    d->ui->nonFreeGroup->setEnabled(enableNonFree);
}

// #include <texteditorplugin/texteditordialog.h>
void DrugsDbModeWidget::onSeeDatabaseDescriptionFileRequested()
{
    // qWarning() << "onSeeDatabaseDescriptionFileRequested()";
    IDrugDatabase *base = d->currentDatabase();
    // qWarning() << base;
    if (!base)
        return;
    // qWarning() << base->databaseDescriptionFile();
//    Editor::TextEditorDialog dlg(this);
//    dlg.readFile(base->databaseDescriptionFile());
//    dlg.exec();
    QDesktopServices::openUrl(QUrl(base->databaseDescriptionFilePath()));
}

void DrugsDbModeWidget::onSeeDatapackDescriptionFileRequested()
{
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;
    // TODO: open file in a TextEditor that allow save & edit
    QDesktopServices::openUrl(QUrl(base->datapackDescriptionFilePath()));
}

/**
 * Reacts on the 'Create Datapack Files' button clicks.
 */
void DrugsDbModeWidget::onCreateDatapackFiles()
{
    // Get the currently selected database
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;

    // Confirmation dialog (YesNo)
    bool yes = Utils::yesNoMessageBox(tr("Create datapack files"),
                           tr("You are about to create datapack files for the "
                              "database: <br><b>%1</b><br>"
                              "These files are only used by the FreeToolBox "
                              "application when creating "
                              "datapack's' server contents.<br><br>"
                              "Do you really want to create the datapack files "
                              "for the database: %1?"
                              ).arg(base->displayName()), "", tr("Datapack Files"));
    if (!yes)
        return;

    // If setting Core::S_DATAPACK_SERVER_OUTPUT_PATH is empty -> Ask for a path
    QString path = settings()->value(Core::Constants::S_DATAPACK_SERVER_OUTPUT_PATH).toString();
    if (path.isEmpty() || !QDir(path).exists()) {
            path = QFileDialog::getExistingDirectory(this, tr("Select a path for the datapack"),
                                                     settings()->path(Core::ISettings::UserDocumentsPath));
            if (path.isEmpty())
                return;
    }

    // Create a specific path with the connection name of the database
    path += QDir::cleanPath(QString("/%1/").arg(base->connectionName().toLower()));
    if (!QDir().mkpath(path)) {
        Utils::warningMessageBox(tr("Error"),
                                 tkTr(Trans::Constants::PATH_1_CANNOT_BE_CREATED)
                                 .arg(path)
                                 );
        return;
    }

    // Update XML datapack description files using the default one (available on the git repo)
    // Set date of update, (authors?), size, md5, sha1, update infos, version
    DataPack::Pack pack;
    pack.fromXmlFile(base->datapackDescriptionFilePath());
    DataPack::PackDescription descr = pack.description();
    QString packVersion = qApp->applicationVersion();
    descr.setData(DataPack::PackDescription::Version, packVersion);
    // TODO: improve version compatibility coding
    descr.setData(DataPack::PackDescription::FreeDiamsCompatVersion, qApp->applicationVersion());
    descr.setData(DataPack::PackDescription::EHRCompatVersion, qApp->applicationVersion());
    descr.setData(DataPack::PackDescription::LastModificationDate, QDate::currentDate().toString(Qt::ISODate));
    descr.setData(DataPack::PackDescription::Size, QFileInfo(base->absoluteFilePath()).size());
    descr.setData(DataPack::PackDescription::Md5, Utils::fileMd5(base->absoluteFilePath()));
    descr.setData(DataPack::PackDescription::Sha1, Utils::fileSha1(base->absoluteFilePath()));
    descr.setData(DataPack::PackDescription::Country, QLocale::countryToString(base->country()));
    Utils::GenericUpdateInformation update;
    update.setDate(QDate::currentDate());
    update.setAuthor("The FreeMedForms Team");
    // TODO: update.setFromVersion();
    update.setToVersion(packVersion);
    update.setText("Complete update of the database. "
                   "For more information, please check the FreeHealth website: "
                   "https://freehealth.io/");
    descr.addUpdateInformation(update);
    pack.setPackDescription(descr);

    // Save the DataPack Description file
    QString newDescriptionFileName = QString("%1/%2").arg(path).arg(DataPack::Constants::PACKDESCRIPTION_FILENAME);
    if (!Utils::saveStringToFile(pack.toXml(), newDescriptionFileName, Utils::Overwrite, Utils::DontWarnUser, 0)) {
        Utils::warningMessageBox(tr("Error"),
                                 tkTr(Trans::Constants::FILE_1_CAN_NOT_BE_CREATED)
                                 .arg(QString("Datapack description at: %1").arg(newDescriptionFileName))
                                 );
        return;
    }

    // Copy database file next to the XML file
    // TODO: remove magic number "master.db"
    QString contentFileName = QString("%1/master.db").arg(path);
    QFile outFile(contentFileName);
    if (outFile.exists()) {
        outFile.remove();
        // TODO: in case of error add dialog
    }
    QFile baseFile(base->absoluteFilePath());
    if (!baseFile.copy(contentFileName)) {
        Utils::warningMessageBox(tr("Error"),
                                 tkTr(Trans::Constants::FILE_1_CAN_NOT_BE_CREATED)
                                 .arg(QString("Datapack content at: %1").arg(contentFileName))
                                 );
        return;
    }

    // Create a PackCreationQueue with the DataPack contents (here a simple unzipped file)
    DataPack::PackCreationQueue queue;
    DataPack::RequestedPackCreation request;
    request.serverUid = base->serverUid();
    request.descriptionFilePath = newDescriptionFileName;
    request.content.insert(DataPack::RequestedPackCreation::UnzippedFile, contentFileName);
    if (!queue.addToQueue(request))
        LOG_ERROR_FOR("PackCreationQueue", "Unable to create CreationQueue file");
    QString queueFileName = QString("%1/%2").arg(path).arg(DataPack::Constants::PACKCREATIONQUEUE_DEFAULT_FILENAME);
    if (!queue.saveToXmlFile(queueFileName, DataPack::PackCreationQueue::UseRelativePath))
        LOG_ERROR_FOR("PackCreationQueue", "Unable to save CreationQueue file");

    // Option: zip database file?

    yes = Utils::yesNoMessageBox(tr("Datapack created"),
                                      tr("Datapack succesfully created. Do you want to "
                                         "open your file explorer to navigate throught "
                                         "the datapack files?"),
                                      "",
                                      tr("Datapack created"));
    if (yes)
        QDesktopServices::openUrl(QUrl(QString("file://%1").arg(path)));

}

/** At any time, user can request a database report */
void DrugsDbModeWidget::onShowDatabaseReportRequested()
{
    // TODO: create a specific report on a DrugDatabase (not the DDIDatabase like the ddiplugin/database/ddidatabasereporter.h
}

void DrugsDbModeWidget::onShowComponentsRequested()
{
    // Get currently selected drugs database
    IDrugDatabase *base = d->currentDatabase();
    if (!base)
        return;

    // Update the component model filter
    ddiCore()->componentAtcModel()->selectDatabase(base->databaseUid1(), base->databaseUid2());

    // Activate Component mode
    modeManager()->activateMode(DDI::Constants::MODE_COMPONENTATC);
}
