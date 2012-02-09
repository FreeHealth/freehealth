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
#include <datapackutils/packmanager.h>
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
static inline DataPack::Internal::PackManager *packManager() { return qobject_cast<Internal::PackManager*>(core().packManager()); }

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
    ui(new Ui::PackProcessDialog),
    m_Error(false)
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

    setServerEngines(serverManager()->serverEngines());

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(pressed()), this, SLOT(processPacks()));
    connect(packManager(), SIGNAL(packDownloaded(Pack,ServerEngineStatus)), this, SLOT(packDownloadDone(Pack, ServerEngineStatus)));
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
{
    m_UpdatePacks = packs;
}

void PackProcessDialog::setPackToRemove(const Pack &pack)
{
    setPackToRemove(QList<Pack>() << pack);
}

void PackProcessDialog::setPackToRemove(const QList<Pack> &packs)
{
    m_RemovePacks = packs;
}

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

void PackProcessDialog::setServerEngines(const QVector<DataPack::IServerEngine*> &engines)
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
    m_Error = false;
    ui->buttonBox->setEnabled(false);
    removePacks();
    startPackDownloads();
}

void PackProcessDialog::startPackDownloads()
{
    Q_ASSERT(!m_Engines.isEmpty());
    m_Msg.clear();

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
        // Create UI widget for the pack
        QLabel *packLabel = new QLabel(dld.at(i).name(), m_ScrollWidget);
        QProgressBar *bar = new QProgressBar(this);
        m_ScrollLayout->addWidget(packLabel, i + 1, 0);
        m_ScrollLayout->addWidget(bar, i + 1, 1);
        packManager()->downloadPack(dld.at(i), bar);
    }
}


void PackProcessDialog::packDownloadDone(const Pack &pack, const ServerEngineStatus &status)
{
    if (status.isSuccessful && !status.hasError) {
        packManager()->installDownloadedPack(pack);
    }
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
        packManager()->removePack(p);
    }
    clearTemporaries();
}

void PackProcessDialog::clearTemporaries()
{
    showLogMessage();
}

void PackProcessDialog::showLogMessage()
{
    ui->buttonBox->setEnabled(true);
    ui->textBrowser->setHtml(m_Msg.join("<br />"));
}
