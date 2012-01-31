/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "packprocessdialog.h"

#include <utils/log.h>
#include <utils/global.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>
#include <datapackutils/iserverengine.h>

#include <quazip/global.h>

#include <QStandardItemModel>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QProgressBar>
#include <QDir>

#include "ui_packprocessdialog.h"

using namespace DataPack;

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline DataPack::Internal::ServerManager *serverManager() { return qobject_cast<DataPack::Internal::ServerManager*>(DataPack::DataPackCore::instance().serverManager()); }

namespace {
const char * const CSS =
        "QTreeView::item:hover {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
        "}"
        "QTreeView::item:selected:active{"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        "QTreeView::item:selected:!active {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}";
}

PackProcessDialog::PackProcessDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PackProcessDialog)
{
    ui->setupUi(this);

    // Manage ScrollArea
    m_ScrollWidget = new QWidget(this);
    m_ScrollLayout = new QGridLayout(m_ScrollWidget);
    m_ScrollWidget->setLayout(m_ScrollLayout);
    ui->scrollArea->setWidget(m_ScrollWidget);
    ui->scrollArea->hide();

    setWindowTitle(ui->titleLabel->text());
    ui->treeWidget->header()->hide();
    ui->treeWidget->setStyleSheet(::CSS);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(pressed()), this, SLOT(processPacks()));

    setServerEngines(serverManager()->serverEngines());
}

PackProcessDialog::~PackProcessDialog()
{
    delete ui;
}

static inline void createBranch(const QString &name, const QList<Pack> &packs, QTreeWidget *widget)
{
    if (packs.isEmpty())
        return;
    QFont bold;
    bold.setBold(true);
    QTreeWidgetItem *root = new QTreeWidgetItem(widget, QStringList() << name);
    root->setFont(0, bold);
    foreach(const Pack &pack, packs) {
        QString l = pack.name() + " " + pack.version();
        new QTreeWidgetItem(root, QStringList() << l);
    }
}

void PackProcessDialog::setPackToInstall(const Pack &pack)
{
    setPackToInstall(QList<Pack>() << pack);
}

void PackProcessDialog::setPackToInstall(const QList<Pack> &packs)
{
}

void PackProcessDialog::setPackToUpdate(const Pack &pack)
{
    setPackToRemove(QList<Pack>() << pack);
}

void PackProcessDialog::setPackToUpdate(const QList<Pack> &packs)
{}

void PackProcessDialog::setPackToRemove(const Pack &pack)
{
    setPackToRemove(QList<Pack>() << pack);
}

void PackProcessDialog::setPackToRemove(const QList<Pack> &packs)
{}

void PackProcessDialog::setPackToProcess(const QList<Pack> &installPacks, const QList<Pack> &updatePacks, const QList<Pack> &removePacks)
{
    m_InstallPacks = installPacks;
    m_UpdatePacks = updatePacks;
    m_RemovePacks = removePacks;

    createBranch(tr("Pack to install"), installPacks, ui->treeWidget);
    createBranch(tr("Pack to update"), updatePacks, ui->treeWidget);
    createBranch(tr("Pack to remove"), removePacks, ui->treeWidget);

    ui->treeWidget->expandAll();
}

//void PackProcessDialog::checkDependencies() const
//{}

//bool PackProcessDialog::isDependenciesAssumed() const
//{}

void PackProcessDialog::setServerEngines(const QVector<Internal::IServerEngine*> &engines)
{
    m_Engines = engines;
}

void PackProcessDialog::done(int result)
{
    if (result==QDialog::Rejected) {
        LOG(QString("Pack installation aborded."));
        /** \todo manage cancellation */
        QDialog::done(result);
        return;
    }
    QDialog::done(result);
}

void PackProcessDialog::processPacks()
{
    startPackDownloads();
}

void PackProcessDialog::startPackDownloads()
{
    Q_ASSERT(!m_Engines.isEmpty());
    // Download all the packs : install && update packs
    QList<Pack> dld, toDld;
    dld << m_InstallPacks;
    dld << m_UpdatePacks;

    ui->scrollArea->show();

    // Add a start download label in the scroll widget
    QLabel *label = new QLabel(tr("Starting downloads"), m_ScrollWidget);
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize() + 2);
    label->setFont(bold);
    m_ScrollLayout->addWidget(label, 0, 0, 0, 10);

    for(int i = 0; i< dld.count(); ++i) {
        // get a server from server manager
        Pack &pack = dld[i];
        Server server = serverManager()->getServerForPack(pack);

        qWarning() << "TESTING Pack" << pack.uuid() << "Server found" << server.uuid();

        if (server.isNull()) {
            LOG_ERROR("No server found for pack " + pack.uuid() + pack.version());
            continue;
        }

        // Pack not already downloaded ?
        if (!serverManager()->isPackInPersistentCache(pack)) {
            toDld << pack;
        }

        // Download the pack from this server
        for(int j=0; j < m_Engines.count(); ++j) {
            Internal::IServerEngine *engine = m_Engines.at(j);
            if (engine->managesServer(server)) {
                // Create the label/progress for the pack
                QLabel *packLabel = new QLabel(pack.name(), m_ScrollWidget);
                QProgressBar *bar = new QProgressBar(this);
                m_ScrollLayout->addWidget(packLabel, i + 1, 0);
                m_ScrollLayout->addWidget(bar, i + 1, 1);

                Internal::ServerEngineQuery query;
                query.downloadPackFile = true;
                query.pack = &pack;
                query.server = &server;
                query.progressBar = bar;
                engine->addToDownloadQueue(query);
                LOG(QString("Adding %1 to %2 download queue").arg(pack.uuid()).arg(server.uuid()));
            }
        }
    }
    // Start downloads
    bool downloading = false;
    for(int i = 0; i < m_Engines.count(); ++i) {
        Internal::IServerEngine *engine = m_Engines.at(i);
        if (engine->downloadQueueCount() > 0) {
            downloading = true;
            connect(engine, SIGNAL(queueDowloaded()), this, SLOT(packDownloadDone()));
            engine->startDownloadQueue();
        }
    }
    if (!downloading)
        packDownloadDone();
}

void PackProcessDialog::packDownloadDone()
{
    // All engine finished ?
    for(int i=0; i<m_Engines.count(); ++i) {
        if (m_Engines.at(i)->downloadQueueCount()>0) {
            return;
        }
    }

    // Some log for debugging purpose
    QList<Pack> dld;
    dld << m_InstallPacks;
    dld << m_UpdatePacks;

    LOG(QString("Requested packs are downloaded in %1").arg(core().persistentCachePath()));
    QStringList logPacks;
    for(int i=0; i < dld.count(); ++i) {
        const Pack &p = dld.at(i);
        logPacks << QString("%1 (%2, %3)")
                    .arg(p.name())
                    .arg(p.uuid())
                    .arg(p.version());
    }
    LOG(QString("Requested packs: %1").arg(logPacks.join("; ")));

    // Copy/Unzip all packs to datapackInstallPath according to PackDescription::InstallToPath
    for(int i=0; i < dld.count(); ++i) {
        const Pack &p = dld.at(i);
        const QString pathTo = p.unzipPackToPath();
        QDir to(pathTo);
        if (!to.exists()) {
            to.mkpath(pathTo);
        }

        /** \todo manage updating packs */

        // Unzip pack to the install path
        if (!QuaZipTools::unzipFile(p.persistentlyCachedZipFileName(), pathTo))
            LOG_ERROR(QString("Unable to unzip pack file %1 to %2").arg(p.persistentlyCachedZipFileName()).arg(pathTo));
        // Add the pack description for future analysis (update, remove...)
        QFile::copy(p.persistentlyCachedXmlConfigFileName(), p.installedXmlConfigFileName());
    }

    // process the removals
    removePacks();
}

void PackProcessDialog::removePacks()
{
//    m_RemovalLabels.clear();
    // Nothing ? go next step
    if (m_RemovePacks.isEmpty()) {
        clearTemporaries();
        return;
    }
    // Remove packs
    // Add a start download label in the scroll widget
    QLabel *label = new QLabel(tr("Deleting packs"), m_ScrollWidget);
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize() + 2);
    label->setFont(bold);
    int r = m_ScrollLayout->rowCount();
    m_ScrollLayout->addWidget(label, r, 0, 0, 10);
    foreach(const Pack &p, m_RemovePacks) {
        // Create a label
        QLabel *label = new QLabel(tr("Deleting packs"), m_ScrollWidget);
        m_ScrollLayout->addWidget(label, ++r, 0, 0, 10);

        // Remove the zipPath used for the pack
        QFileInfo zipPath(p.unzipPackToPath());
        if (!zipPath.exists()) {
            LOG_ERROR(QString("Unable to remove pack %1, unzip path does not exists (%2)").arg(p.name().arg(p.unzipPackToPath())));
            continue;
        }
        QString error;
        Utils::removeDirRecursively(p.unzipPackToPath(), &error);
        if (!error.isEmpty()) {
            LOG_ERROR(QString("Unable to remove pack %1, error: %2").arg(p.name().arg(error)));
        }
    }
    clearTemporaries();
}

void PackProcessDialog::clearTemporaries()
{
    // Draft
    accept();
}

