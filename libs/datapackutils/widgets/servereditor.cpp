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
/**
  \class DataPack::ServerEditor
  Widget used to modify the server list of the current DataPack::IServerManager
*/

#include "servereditor.h"
#include "addserverdialog.h"

#include <utils/global.h>
#include <utils/widgets/htmldelegate.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include <datapackutils/core.h>
#include <datapackutils/servermanager.h>
#include <datapackutils/serverandpackmodel.h>

#include <QStandardItemModel>
#include <QToolBar>

#include <QDebug>

#include "ui_servereditor.h"

using namespace DataPack;
using namespace Trans::ConstantTranslations;

static inline DataPack::Core *core() {return DataPack::Core::instance();}
static inline Internal::ServerManager *serverManager() {return qobject_cast<Internal::ServerManager*>(core()->serverManager());}
static inline QIcon icon(const QString &name, DataPack::Core::ThemePath path = DataPack::Core::MediumPixmaps) {return QIcon(DataPack::Core::instance()->icon(name, path));}

namespace {

const char *const ICON_SERVER_ADD = "server-add.png";
const char *const ICON_SERVER_REMOVE = "server-remove.png";
const char *const ICON_SERVER_INFO = "server-info.png";

const char *const ICON_UPDATE = "updateavailable.png";
const char *const ICON_INSTALL = "install-package.png";
const char *const ICON_REMOVE = "remove.png";

const char * const TITLE_CSS = "text-indent:5px;padding:5px;font-weight:bold;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))";

const char * const CSS =
        "QserverView::item:hover {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
        "}"
        "QserverView::item:selected:active{"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        "QserverView::item:selected:!active {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}";

}  // End anonymous namespace

ServerEditor::ServerEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerEditor),
    aServerRemove(0), aServerAdd(0),aServerInfo(0),
    aInstall(0), aInstallAllPack(0), aRemove(0), aUpdate(0)
{
    ui->setupUi(this);
    if (layout()) {
        layout()->setMargin(0);
        layout()->setSpacing(0);
        ui->toolbarLayout->setMargin(0);
        ui->toolbarLayout->setSpacing(0);
    }

    // Manage server view
    m_ServerModel = new ServerAndPackModel(this);
    m_ServerModel->setPackCheckable(true);
    m_ServerModel->setInstallChecker(true);
    m_ServerModel->setColumnCount(1);
    ui->serverView->setModel(m_ServerModel);
//    ui->serverView->
    Utils::HtmlDelegate *delegate = new Utils::HtmlDelegate;
    ui->serverView->setItemDelegate(delegate);
    ui->serverView->setStyleSheet(::CSS);
//    ui->serverView->expandAll();
    ui->serverView->setAlternatingRowColors(true);
//    ui->serverView->setRootIsDecorated(false);
    ui->serverView->setEditTriggers(QTreeView::NoEditTriggers);

    // Manage pack view
    ui->packView->setModel(m_ServerModel);
//    ui->packView->header()->hide();
    ui->packView->setItemDelegate(delegate);
    ui->packView->setStyleSheet(::CSS);
//    ui->packView->expandAll();
//    ui->packView->setRootIsDecorated(false);
//    ui->packView->setAlternatingRowColors(true);
    ui->packView->setEditTriggers(QTreeView::NoEditTriggers);

    // Manage central view
    ui->stackedWidget->setCurrentIndex(0);
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize()+1);
    ui->serverName->setFont(bold);
    ui->packName->setFont(bold);
    ui->serverName->setStyleSheet(::TITLE_CSS);
    ui->packName->setStyleSheet(::TITLE_CSS);

    createActions();
    createToolbar();

    connect(ui->serverView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onServerIndexActivated(QModelIndex,QModelIndex)));
    connect(ui->packView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onPackIndexActivated(QModelIndex,QModelIndex)));
    connect(serverManager(), SIGNAL(serverAdded(int)), this, SLOT(serverAdded(int)));
    connect(serverManager(), SIGNAL(serverRemoved(int)), this, SLOT(serverRemoved(int)));

    serverManager()->getAllDescriptionFile();

    // Select first row of servers
    ui->serverView->selectionModel()->select(m_ServerModel->index(0,0), QItemSelectionModel::SelectCurrent);
}

ServerEditor::~ServerEditor()
{
    delete ui;
}

bool ServerEditor::submitChanges()
{
    return true;
}

void ServerEditor::createActions()
{
    // Create server actions
    QAction *a = aServerAdd = new QAction(this);
    a->setObjectName("aInstall");
    a->setIcon(icon(::ICON_SERVER_ADD, DataPack::Core::MediumPixmaps));
    a = aServerRemove = new QAction(this);
    a->setObjectName("aServerRemove");
    a->setIcon(icon(::ICON_SERVER_REMOVE, DataPack::Core::MediumPixmaps));
    a = aServerInfo = new QAction(this);
    a->setObjectName("aServerInfo");
    a->setIcon(icon(::ICON_SERVER_INFO, DataPack::Core::MediumPixmaps));

    // Create pack actions
    a = aInstall = new QAction(this);
    a->setObjectName("aInstall");
    a->setIcon(icon(::ICON_INSTALL, DataPack::Core::SmallPixmaps));
    a = aInstallAllPack = new QAction(this);
    a->setObjectName("aInstallAllPack");
    a->setIcon(icon(::ICON_INSTALL, DataPack::Core::MediumPixmaps));
    a = aUpdate = new QAction(this);
    a->setObjectName("aUpdate");
    a->setIcon(icon(::ICON_UPDATE, DataPack::Core::SmallPixmaps));
    a = aRemove = new QAction(this);
    a->setObjectName("aRemove");
    a->setIcon(icon(::ICON_REMOVE, DataPack::Core::SmallPixmaps));
    ui->installToolButton->addAction(aInstall);
    ui->installToolButton->addAction(aUpdate);
    ui->installToolButton->addAction(aRemove);
    ui->installToolButton->setDefaultAction(aInstall);
    retranslate();
    connect(ui->installToolButton, SIGNAL(triggered(QAction*)), this, SLOT(packActionTriggered(QAction *)));
}

void ServerEditor::createToolbar()
{
    m_ToolBar = new QToolBar(this);
    m_ToolBar->addAction(aServerAdd);
    m_ToolBar->addAction(aServerRemove);
    m_ToolBar->addAction(aServerInfo);
    m_ToolBar->addSeparator();
    m_ToolBar->addAction(aInstallAllPack);
    connect(m_ToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(serverActionTriggered(QAction*)));
    ui->toolbarLayout->addWidget(m_ToolBar);
}

static void elideTextToLabel(QLabel *label, const QString &text)
{
    if (text.size() > 30) {
        label->setText(text.left(27)+"...");
        QString u = text;
        int i = 30;
        while (i < text.size()) {
            u.insert(i, "\n");
            i += 32;
        }
        label->setToolTip(u);
    } else {
        label->setText(text);
        label->setToolTip(text);
    }
}

void ServerEditor::populateServerView(const int serverId)
{
    // Clear pack selection
    ui->packView->selectionModel()->clearSelection();
    ui->packView->setRootIndex(m_ServerModel->index(serverId, 0));

    // Update server view
    const QString &format = QLocale().dateTimeFormat(QLocale::LongFormat);
    const Server &server = serverManager()->getServerAt(serverId);
    const QString &uuid = server.uuid();
    ui->serverName->setText(server.description().data(ServerDescription::Label).toString());
    ui->shortServerDescription->setText(server.description().data(ServerDescription::ShortDescription).toString());
    ui->serverVersion->setText(server.description().data(ServerDescription::Version).toString());
    ui->serverDate->setText(server.description().data(ServerDescription::LastModificationDate).toDateTime().toString(format));
    ui->serverDescription->setText(server.description().data(ServerDescription::ShortDescription).toString());
    ui->lastCheck->setText(server.lastChecked().toString(format));
    ui->serverVendor->setText(server.description().data(ServerDescription::Vendor).toString());
    elideTextToLabel(ui->serverUuid, server.uuid());
    elideTextToLabel(ui->serverNativeUrl, server.nativeUrl());
    if (server.isConnected() || server.isLocalServer()) {
        ui->serverState->setText(tkTr(Trans::Constants::CONNECTED));
    } else {
        ui->serverState->setText(tkTr(Trans::Constants::NOT_CONNECTED));
    }
}

void ServerEditor::populatePackView(const int serverId, const int packId)
{
    const Server &server = serverManager()->getServerAt(serverId);
    const QList<Pack> &list = serverManager()->getPackForServer(server);
    const Pack &pack = list.at(packId);
    const PackDescription &descr = pack.description();
    // Vendor ?
    QString vendor = descr.data(PackDescription::Vendor).toString();
    if (!vendor.isEmpty()) {
        vendor = QString("<br /><span style=\"font-size:small;color:gray\">%1</span>").arg(vendor);
    }
    QString summary;

    // short description, version date and author
    summary = QString("<span style=\"font-weight:bold;font-size:large;\">%1</span><br />"
                       "<span style=\"font-size:small;color:gray\">"
                      "%2 (%3 - %4) from %5</span>"
                       "%6")
            .arg(descr.data(PackDescription::ShortDescription).toString())
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(descr.data(PackDescription::Version).toString())
            .arg(descr.data(PackDescription::LastModificationDate).toDate().toString("dd MM yyyy"))
            .arg(descr.data(PackDescription::Author).toString())
            .arg(vendor)
            ;

    // Add description
    summary += QString("<br /><br />%1").arg(descr.data(PackDescription::HtmlDescription).toString());

    // Add dependencies
    QString dep;
    for(int i= 0; i < pack.dependencies().dependenciesCount(); ++i) {
        const PackDependencyData &data = pack.dependencies().dependenciesAt(i);
        dep += QString("<p style=\"margin-left:10px;\">%1<br />"
                       "&nbsp;&nbsp;&nbsp;&nbsp;%2 (%3)</p>")
                .arg(PackDependencyData::typeName(data.type()))
                .arg(data.name())
                .arg(data.version())
                ;
    }
    if (!dep.isEmpty()) {
        dep.prepend(QString("<span style=\"font-weight:bold;margin:0px;\">%1</span>").arg(tr("Dependencies")));
        summary += dep + "<br />";
    }

    // Add file specifications
    QString file = QString("<span style=\"font-weight:bold\">%1</span><br />"
                           "<table border=1 cellpadding=0 cellspacing=0 width=100%>"
                           "<tr><td>%2</td><td>%3</td></tr>"
                           "<tr><td>MD5</td><td>%4</td></tr>"
                           "<tr><td>SHA1</td><td>%5</td></tr>"
                           "</table>")
            .arg(tr("File specification"))
            .arg(tr("File name or URL:"))
            .arg(descr.data(PackDescription::AbsFileName).toString())
            .arg(descr.data(PackDescription::Md5).toString())
            .arg(descr.data(PackDescription::Sha1).toString())
            ;

    summary += file;

    ui->packName->setText(descr.data(PackDescription::Label).toString());
    ui->packSummary->setText(summary);
}

void ServerEditor::onServerIndexActivated(const QModelIndex &index, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    // Activate the Server Page
    ui->stackedWidget->setCurrentWidget(ui->serverPage);
    populateServerView(index.row());
}

void ServerEditor::onPackIndexActivated(const QModelIndex &index, const QModelIndex &previous)
{
    // Activate the Server Page
    ui->stackedWidget->setCurrentWidget(ui->packPage);
    int serverId = ui->serverView->selectionModel()->currentIndex().row();
    populatePackView(serverId, index.row());
}

void ServerEditor::serverActionTriggered(QAction *a)
{
    if (a==aServerAdd) {
        AddServerDialog dlg(this);
        Server server;
        dlg.setServer(server);
        if (dlg.exec()==QDialog::Accepted) {
            dlg.submitTo(&server);
            serverManager()->addServer(server);
        }
    } else if (a==aServerRemove) {
        /** \todo code here */
    } else if (a==aServerInfo) {
        // Clear pack selection
        ui->packView->selectionModel()->clearSelection();
        ui->stackedWidget->setCurrentWidget(ui->serverPage);
    }
}

void ServerEditor::packActionTriggered(QAction *a)
{
    int serverId = ui->serverView->selectionModel()->currentIndex().row();
    int packId = ui->packView->selectionModel()->currentIndex().row();
    const Server &server = serverManager()->getServerAt(serverId);
    QList<Pack> list = serverManager()->getPackForServer(server);
    const Pack &pack = list.at(packId);

    if (a==aInstall) {
        serverManager()->installDataPack(server, pack, ui->processProgressBar);
    } else if (a==aRemove) {

    } else if (a==aUpdate) {

    }
}

void ServerEditor::serverAdded(int i)
{
    Q_UNUSED(i);
    // Refresh the model
//    createServerModel(m_ServerModel);
}

void ServerEditor::serverRemoved(int i)
{
    Q_UNUSED(i);
    // Refresh the model
//    createServerModel(m_ServerModel);
}

void ServerEditor::retranslate()
{
    aServerAdd->setText(tr("Add a server"));
    aServerRemove->setText(tr("Remove a server"));
    aServerInfo->setText(tr("Server information"));
    aInstall->setText(tkTr(Trans::Constants::INSTALL));
    aInstallAllPack->setText(tr("Install all pack"));
    aRemove->setText(tkTr(Trans::Constants::REMOVE_TEXT));
    aUpdate->setText(tkTr(Trans::Constants::UPDATE));
}
void ServerEditor::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslate();
    }
}
