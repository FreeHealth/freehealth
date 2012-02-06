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
/**
  \class DataPack::ServerPackEditor
  Widget used to modify the server list and the installed packs.
*/

#include "serverpackeditor.h"
#include "addserverdialog.h"

#include <utils/global.h>
#include <utils/widgets/htmldelegate.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>
#include <datapackutils/packmanager.h>
#include <datapackutils/packmodel.h>
#include <datapackutils/servermodel.h>
#include <datapackutils/widgets/packprocessdialog.h>

#include <QToolBar>
#include <QProgressDialog>
#include <QProgressBar>
#include <QListWidgetItem>
#include <QDataWidgetMapper>

#include <QDebug>

#include "ui_serverpackeditor.h"

using namespace DataPack;
using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline Internal::ServerManager *serverManager() { return qobject_cast<Internal::ServerManager*>(core().serverManager()); }
static inline Internal::PackManager *packManager() { return qobject_cast<Internal::PackManager*>(core().packManager()); }
static inline QIcon icon(const QString &name, DataPack::DataPackCore::ThemePath path = DataPack::DataPackCore::MediumPixmaps) { return QIcon(core().icon(name, path)); }

namespace {

const char *const ICON_SERVER_REFRESH = "datapack-server-refresh.png";
const char *const ICON_SERVER_EDIT = "datapack-server-edit.png";
const char *const ICON_SERVER_ADD = "server-add.png";
const char *const ICON_SERVER_REMOVE = "server-remove.png";
const char *const ICON_SERVER_INFO = "server-info.png";

const char *const ICON_UPDATE = "updateavailable.png";
const char *const ICON_INSTALL = "install-package.png";
const char *const ICON_REMOVE = "remove.png";

const char *const ICON_PACKAGE = "package.png";

const char * const TITLE_CSS = "text-indent:5px;padding:5px;font-weight:bold;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))";

const char * const CSS =
        "serverView::item:hover {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
        "}"
        "serverView::item:selected:active{"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        "serverView::item:selected:!active {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}";

const int SERVER_MODE = 0;
const int PACK_MODE = 1;

}  // End anonymous namespace

ServerPackEditor::ServerPackEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerPackEditor),
    aServerRefresh(0), aServerRemove(0), aServerAdd(0),
    aProcess(0)
{
    setObjectName("ServerPackEditor");
    ui->setupUi(this);

    if (layout()) {
        layout()->setMargin(0);
        layout()->setSpacing(0);
        ui->toolbarLayout->setMargin(0);
        ui->toolbarLayout->setSpacing(0);
        for(int i=0; i<ui->stackedWidget->count(); ++i) {
            QWidget *w = ui->stackedWidget->widget(i);
            if (w->layout())
                w->layout()->setMargin(0);
        }
    }

    // Left menu items
    m_datapacksItem = new QListWidgetItem(icon(::ICON_INSTALL), tr("Datapacks"));
    ui->listWidgetMenu->addItem(m_datapacksItem);
    m_serversItem = new QListWidgetItem(icon(::ICON_SERVER_ADD), tr("Servers"));
    ui->listWidgetMenu->addItem(m_serversItem);
    ui->listWidgetMenu->setCurrentRow(0);
    ui->listWidgetMenu->hide();

    // Manage pack model/view
    m_PackModel = new PackModel(this);
    m_PackModel->setPackCheckable(true);
    m_PackModel->setInstallChecker(true);
    ui->packView->setModel(m_PackModel);
    ui->packView->setModelColumn(PackModel::Label);

    Utils::HtmlDelegate *delegate = new Utils::HtmlDelegate;
    ui->packView->setItemDelegate(delegate);
    ui->packView->setStyleSheet(::CSS);
    //    ui->packView->header()->hide();
    ui->packView->setAlternatingRowColors(true);

    // server page
    m_serverModel = new ServerModel(this);
    ui->serverView->setModel(m_serverModel);
    ui->serverView->setModelColumn(ServerModel::HtmlLabel);
    ui->serverView->setItemDelegate(delegate);
    ui->serverView->setStyleSheet(::CSS);
    ui->serverView->setAlternatingRowColors(true);

    // Manage central view
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize()+1);
    ui->packName->setFont(bold);
    ui->packName->setStyleSheet(::TITLE_CSS);
    ui->serverLabel->setStyleSheet(::TITLE_CSS);

    createActions();
    createToolbar();
    processToolBar(::PACK_MODE);
    createServerDataWidgetMapper();

    connect(ui->packView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onPackIndexActivated(QModelIndex,QModelIndex)));
    connect(ui->serverView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(serverCurrentChanged(QModelIndex,QModelIndex)));

    ui->stackedWidget->setCurrentWidget(ui->pagePacks);

    // FOR TEST
    // This step should be done before the ServerPackEditor in called
    serverManager()->getAllDescriptionFile();
    // END TEST

    // file://Users/eric/Desktop/Programmation/freemedforms/global_resources/datapacks/default/
}

ServerPackEditor::~ServerPackEditor()
{
    delete ui;
}

bool ServerPackEditor::submitChanges()
{
    return true;
}

void ServerPackEditor::createActions()
{
    // Create server actions
    QActionGroup *srvgr = new QActionGroup(this);
    QAction *a = aServerRefresh = new QAction(this);
    a->setObjectName("aServerRefresh");
    a->setIcon(icon(::ICON_SERVER_REFRESH, DataPack::DataPackCore::MediumPixmaps));
    a = aServerEdit = new QAction(this);
    a->setObjectName("aServerEdit");
    a->setIcon(icon(::ICON_SERVER_EDIT, DataPack::DataPackCore::MediumPixmaps));
    a = aServerAdd = new QAction(this);
    a->setObjectName("aInstall");
    a->setIcon(icon(::ICON_SERVER_ADD, DataPack::DataPackCore::MediumPixmaps));
    a = aServerRemove = new QAction(this);
    a->setObjectName("aServerRemove");
    a->setIcon(icon(::ICON_SERVER_REMOVE, DataPack::DataPackCore::MediumPixmaps));
    connect(srvgr, SIGNAL(triggered(QAction*)), this, SLOT(serverActionTriggered(QAction *)));

    // Create pack actions
    a = aServerPackEditor = new QAction(this);
    a->setObjectName("aServerPackEditor");
    a->setIcon(icon(::ICON_PACKAGE, DataPack::DataPackCore::MediumPixmaps));
    a = aProcess = new QAction(this);
    a->setObjectName("aProcess");
    a->setIcon(icon(::ICON_INSTALL, DataPack::DataPackCore::MediumPixmaps));
//    ui->installToolButton->addAction(aInstall);
//    ui->installToolButton->addAction(aUpdate);
//    ui->installToolButton->addAction(aRemove);
//    ui->installToolButton->setDefaultAction(aInstall);
    retranslate();
    connect(aProcess, SIGNAL(triggered()), this, SLOT(processPacks()));
}

void ServerPackEditor::createToolbar()
{
    m_ToolBarPacks = new QToolBar(this);
    // Insert action in the same order as the enum ToolBarActions
    m_ToolBarPacks->addAction(aServerEdit);
    m_ToolBarPacks->addAction(aServerPackEditor);
    m_ToolBarPacks->addSeparator();
    m_ToolBarPacks->addAction(aServerRefresh);
    m_ToolBarPacks->addSeparator();
    connect(m_ToolBarPacks, SIGNAL(actionTriggered(QAction*)), this, SLOT(serverActionTriggered(QAction*)));
    ui->toolbarLayout->addWidget(m_ToolBarPacks);
}

void ServerPackEditor::createServerDataWidgetMapper()
{
    m_ServerMapper = new QDataWidgetMapper(this);
    m_ServerMapper->setModel(m_serverModel);
    m_ServerMapper->addMapping(ui->serverLabel, ServerModel::PlainTextLabel, "text");
    m_ServerMapper->addMapping(ui->serverVersion, ServerModel::Version, "text");
    m_ServerMapper->addMapping(ui->serverAuthors, ServerModel::Authors, "text");
    m_ServerMapper->addMapping(ui->serverCreationDate, ServerModel::CreationDate, "text");
    m_ServerMapper->addMapping(ui->serverVendor, ServerModel::Vendor, "text");
    m_ServerMapper->addMapping(ui->serverDescription, ServerModel::HtmlDescription, "html");
    m_ServerMapper->setCurrentIndex(1);
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

void ServerPackEditor::populatePackView(const int packId)
{
    const Pack &pack = m_PackModel->packageAt(packId);
    const PackDescription &descr = pack.description();
    // Vendor ?
    QString vendor = descr.data(PackDescription::Vendor).toString();
    if (vendor.isEmpty()) {
        vendor = tr("The FreeMedForms community");
    }
    QString summary;

    // short description, version date and author
    summary = QString("<p style=\"font-weight:bold;font-size:large;\">%1</p>"
                      "<p style=\"font-size:small;margin-left:20px;color:gray\">"
                      "%2: %3<br />"
                      "%4: %5<br />"
                      "%6: %7<br />"
                      "%8: %9"
                      "</p>"
                       )
            .arg(descr.data(PackDescription::ShortDescription).toString())
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(descr.data(PackDescription::Version).toString())
            .arg(tr("Last modification"))
            .arg(descr.data(PackDescription::LastModificationDate).toDate().toString("dd MM yyyy"))
            .arg(tkTr(Trans::Constants::AUTHOR))
            .arg(descr.data(PackDescription::Author).toString())
            .arg(tkTr(Trans::Constants::VENDOR))
            .arg(vendor)
            ;

    // Add description
    summary += descr.data(PackDescription::HtmlDescription).toString();

    // Add update informations
    bool isUpdate = m_PackModel->index(packId, PackModel::IsAnUpdate).data().toBool();
    if (isUpdate) {
        QString up = QString("<p style=\"font-size:normal;margin-left:10px;color:darkblue\">%1</p><br />")
                     .arg(descr.htmlUpdateInformationForVersion("0.0.0"));
        up.prepend(QString("<span style=\"font-weight:bold;margin:0px;\">%1</span>").arg(tr("Update information")));
        summary += up;
    }

    // Add dependencies
    QString dep;
    for(int i= 0; i < pack.dependencies().count(); ++i) {
        const PackDependencyData &data = pack.dependencies().at(i);
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

void ServerPackEditor::onPackIndexActivated(const QModelIndex &index, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    populatePackView(index.row());
}

void ServerPackEditor::processToolBar(int mode)
{
    if (mode==::SERVER_MODE) {
        m_ToolBarPacks->removeAction(aProcess);
        m_ToolBarPacks->addAction(aServerAdd);
        m_ToolBarPacks->addAction(aServerRemove);
    } else if (mode==::PACK_MODE) {
        m_ToolBarPacks->removeAction(aServerAdd);
        m_ToolBarPacks->removeAction(aServerRemove);
        m_ToolBarPacks->addAction(aProcess);
    }
}

void ServerPackEditor::serverActionTriggered(QAction *a)
{
    if (a==aServerRefresh) {
        QProgressDialog dlg(this);
        QProgressBar *bar = new QProgressBar;
        dlg.setLabelText(tr("Updating server information"));
        dlg.setModal(true);
        dlg.setBar(bar);
        dlg.show();
        /** \todo Connect the cancel button */
        serverManager()->getAllDescriptionFile(bar);
    } if (a==aServerAdd) {
        AddServerDialog dlg(this);
        Server server;
        dlg.setServer(server);
        if (dlg.exec()==QDialog::Accepted) {
            dlg.submitTo(&server);
            serverManager()->addServer(server);
            /** \todo manage progress dialog cancellation */
            QProgressDialog dlg(this);
            dlg.setLabelText(tr("Downloading server information"));
            dlg.setModal(true);
            connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), &dlg, SLOT(accept()));
//            connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), dlg, SLOT(close()));
            serverManager()->getServerDescription(serverManager()->serverCount() - 1);
            dlg.exec();
        }
    } else if (a==aServerRemove) {
        if (!ui->serverView->selectionModel()->hasSelection())
            return;
        int row = ui->serverView->selectionModel()->currentIndex().row();
        serverManager()->removeServerAt(row);
    } else if (a==aServerEdit) {
        ui->stackedWidget->setCurrentWidget(ui->pageServers);
        processToolBar(::SERVER_MODE);
    } else if (a==aServerPackEditor) {
        ui->stackedWidget->setCurrentWidget(ui->pagePacks);
        processToolBar(::PACK_MODE);
    }
}

/** Start to process the user pack modification. */
void ServerPackEditor::processPacks()
{
    // Apply pack model changes
    if (!m_PackModel->isDirty())
        return;
    // Run Pack Dialog
    PackProcessDialog dlg;
    dlg.setPackToProcess(m_PackModel->packageToInstall(), m_PackModel->packageToUpdate(), m_PackModel->packageToRemove());
    if (dlg.exec()==QDialog::Rejected) {
        return;
    }
    packManager()->installedPack(true);
    m_PackModel->updateModel();
}

void ServerPackEditor::retranslate()
{
    aServerRefresh->setText(tr("Refresh datapack servers"));
    aServerEdit->setText(tr("Server editor"));
    aServerAdd->setText(tr("Add a server"));
    aServerRemove->setText(tr("Remove a server"));
    aServerPackEditor->setText(tr("Server and pack editor"));
    aProcess->setText(tr("Process changes"));
}

void ServerPackEditor::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslate();
    }
}

void ServerPackEditor::on_listWidgetMenu_currentRowChanged(int row)
{
    if (ui->listWidgetMenu->currentItem() == m_datapacksItem)
        ui->stackedWidget->setCurrentWidget(ui->pagePacks);
    else if (ui->listWidgetMenu->currentItem() == m_serversItem)
        ui->stackedWidget->setCurrentWidget(ui->pageServers);
}

void ServerPackEditor::serverCurrentChanged(const QModelIndex &c, const QModelIndex &p)
{
    m_ServerMapper->setCurrentIndex(c.row());
}
