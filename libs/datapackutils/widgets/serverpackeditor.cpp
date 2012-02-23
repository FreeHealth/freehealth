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
#include <datapackutils/widgets/packwizard.h>

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


namespace DataPack {
namespace Internal {
class ServerPackEditorPrivate
{
public:
    ServerPackEditorPrivate() :
        ui(new Ui::ServerPackEditor),
        m_PackModel(0),
        m_serverModel(0),
        aServerRefresh(0), aServerEdit(0), aServerRemove(0), aServerAdd(0),
        aServerPackEditor(0), aProcess(0),
        m_ToolBarPacks(0),
        m_datapacksItem(0),
        m_serversItem(0),
        m_ServerMapper(0)
    {}

    void processToolBar(int mode)
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

public:
    Ui::ServerPackEditor *ui;
    PackModel *m_PackModel;
    ServerModel *m_serverModel;
    QAction *aServerRefresh, *aServerEdit, *aServerRemove, *aServerAdd;
    QAction *aServerPackEditor, *aProcess;
    QToolBar *m_ToolBarPacks;
    QListWidgetItem *m_datapacksItem;
    QListWidgetItem *m_serversItem;
    QDataWidgetMapper *m_ServerMapper;
};
} // Internal
} // DataPack

ServerPackEditor::ServerPackEditor(QWidget *parent) :
    QWidget(parent),
    d(new Internal::ServerPackEditorPrivate)
{
    setObjectName("ServerPackEditor");
    d->ui->setupUi(this);

    if (layout()) {
        layout()->setMargin(0);
        layout()->setSpacing(0);
        d->ui->toolbarLayout->setMargin(0);
        d->ui->toolbarLayout->setSpacing(0);
        for(int i=0; i<d->ui->stackedWidget->count(); ++i) {
            QWidget *w = d->ui->stackedWidget->widget(i);
            if (w->layout())
                w->layout()->setMargin(0);
        }
    }

    // Left menu items
    d->m_datapacksItem = new QListWidgetItem(icon(::ICON_INSTALL), tr("Datapacks"));
    d->ui->listWidgetMenu->addItem(d->m_datapacksItem);
    d->m_serversItem = new QListWidgetItem(icon(::ICON_SERVER_ADD), tr("Servers"));
    d->ui->listWidgetMenu->addItem(d->m_serversItem);
    d->ui->listWidgetMenu->setCurrentRow(0);
    d->ui->listWidgetMenu->hide();

    // Manage pack model/view
    d->m_PackModel = new PackModel(this);
    d->m_PackModel->setPackCheckable(true);
    d->m_PackModel->setInstallChecker(true);
    d->ui->packView->setModel(d->m_PackModel);
    d->ui->packView->setModelColumn(PackModel::Label);

    Utils::HtmlDelegate *delegate = new Utils::HtmlDelegate;
    d->ui->packView->setItemDelegate(delegate);
    d->ui->packView->setStyleSheet(::CSS);
    //    d->ui->packView->header()->hide();
    d->ui->packView->setAlternatingRowColors(true);

    // server page
    d->m_serverModel = new ServerModel(this);
    d->ui->serverView->setModel(d->m_serverModel);
    d->ui->serverView->setModelColumn(ServerModel::HtmlLabel);
    d->ui->serverView->setItemDelegate(delegate);
    d->ui->serverView->setStyleSheet(::CSS);
    d->ui->serverView->setAlternatingRowColors(true);

    // Manage central view
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize()+1);
    d->ui->packName->setFont(bold);
    d->ui->packName->setStyleSheet(::TITLE_CSS);
    d->ui->serverLabel->setStyleSheet(::TITLE_CSS);

    createActions();
    createToolbar();
    d->processToolBar(::PACK_MODE);
    createServerDataWidgetMapper();

    connect(d->ui->packView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onPackIndexActivated(QModelIndex,QModelIndex)));
    connect(d->ui->serverView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(serverCurrentChanged(QModelIndex,QModelIndex)));

    d->ui->stackedWidget->setCurrentWidget(d->ui->pagePacks);

    // file://Users/eric/Desktop/Programmation/freemedforms/global_resources/datapacks/default/
}

ServerPackEditor::~ServerPackEditor()
{
    if (d) {
        delete d;
        d=0;
    }
}

bool ServerPackEditor::refreshServerContent()
{
    if (serverManager()->serverCount() == 0)
        return true;
    QProgressDialog dlg(this);
    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), &dlg, SLOT(close()));
    QProgressBar *bar = new QProgressBar;
    dlg.setLabelText(tr("Updating server information"));
    dlg.setModal(true);
    dlg.setBar(bar);
    dlg.show();
    connect(&dlg, SIGNAL(canceled()), &core(), SLOT(stopJobsAndClearQueues()));
    /** \todo Connect the cancel button */
    serverManager()->getAllDescriptionFile(bar);
//    dlg.exec();
    return true;
}

bool ServerPackEditor::submitChanges()
{
    return true;
}

void ServerPackEditor::createActions()
{
    // Create server actions
    QActionGroup *srvgr = new QActionGroup(this);
    QAction *a = d->aServerRefresh = new QAction(this);
    a->setObjectName("aServerRefresh");
    a->setIcon(icon(::ICON_SERVER_REFRESH, DataPack::DataPackCore::MediumPixmaps));
    a = d->aServerEdit = new QAction(this);
    a->setObjectName("aServerEdit");
    a->setIcon(icon(::ICON_SERVER_EDIT, DataPack::DataPackCore::MediumPixmaps));
    a = d->aServerAdd = new QAction(this);
    a->setObjectName("aInstall");
    a->setIcon(icon(::ICON_SERVER_ADD, DataPack::DataPackCore::MediumPixmaps));
    a = d->aServerRemove = new QAction(this);
    a->setObjectName("aServerRemove");
    a->setIcon(icon(::ICON_SERVER_REMOVE, DataPack::DataPackCore::MediumPixmaps));
    connect(srvgr, SIGNAL(triggered(QAction*)), this, SLOT(serverActionTriggered(QAction *)));

    // Create pack actions
    a = d->aServerPackEditor = new QAction(this);
    a->setObjectName("aServerPackEditor");
    a->setIcon(icon(::ICON_PACKAGE, DataPack::DataPackCore::MediumPixmaps));
    a = d->aProcess = new QAction(this);
    a->setObjectName("aProcess");
    a->setIcon(icon(::ICON_INSTALL, DataPack::DataPackCore::MediumPixmaps));
//    d->ui->installToolButton->addAction(aInstall);
//    d->ui->installToolButton->addAction(aUpdate);
//    d->ui->installToolButton->addAction(aRemove);
//    d->ui->installToolButton->setDefaultAction(aInstall);
    retranslate();
    connect(d->aProcess, SIGNAL(triggered()), this, SLOT(processPacks()));
}

void ServerPackEditor::createToolbar()
{
    d->m_ToolBarPacks = new QToolBar(this);
    // Insert action in the same order as the enum ToolBarActions
    d->m_ToolBarPacks->addAction(d->aServerEdit);
    d->m_ToolBarPacks->addAction(d->aServerPackEditor);
    d->m_ToolBarPacks->addSeparator();
    d->m_ToolBarPacks->addAction(d->aServerRefresh);
    d->m_ToolBarPacks->addSeparator();
    connect(d->m_ToolBarPacks, SIGNAL(actionTriggered(QAction*)), this, SLOT(serverActionTriggered(QAction*)));
    d->ui->toolbarLayout->addWidget(d->m_ToolBarPacks);
}

void ServerPackEditor::createServerDataWidgetMapper()
{
    d->m_ServerMapper = new QDataWidgetMapper(this);
    d->m_ServerMapper->setModel(d->m_serverModel);
    d->m_ServerMapper->addMapping(d->ui->serverLabel, ServerModel::PlainTextLabel, "text");
    d->m_ServerMapper->addMapping(d->ui->serverVersion, ServerModel::Version, "text");
    d->m_ServerMapper->addMapping(d->ui->serverAuthors, ServerModel::Authors, "text");
    d->m_ServerMapper->addMapping(d->ui->serverCreationDate, ServerModel::CreationDate, "text");
    d->m_ServerMapper->addMapping(d->ui->serverVendor, ServerModel::Vendor, "text");
    d->m_ServerMapper->addMapping(d->ui->serverDescription, ServerModel::HtmlDescription, "html");
    d->m_ServerMapper->setCurrentIndex(1);
}

//static void elideTextToLabel(QLabel *label, const QString &text)
//{
//    if (text.size() > 30) {
//        label->setText(text.left(27)+"...");
//        QString u = text;
//        int i = 30;
//        while (i < text.size()) {
//            u.insert(i, "\n");
//            i += 32;
//        }
//        label->setToolTip(u);
//    } else {
//        label->setText(text);
//        label->setToolTip(text);
//    }
//}

void ServerPackEditor::populatePackView(const int packId)
{
    const Pack &pack = d->m_PackModel->packageAt(packId);
    const PackDescription &descr = pack.description();
    QString summary;

    // short description, version date and author
    summary = QString("<p style=\"font-weight:bold;font-size:large;\">%1</p>"
                      "<p style=\"font-size:small;margin-left:20px;color:gray\">"
                      "%2: %3<br />"
                      "%4: %5<br />"
                      "%6: %7<br />"
                      "%8: %9<br />"
                      "%10: %11"
                      "</p>"
                       )
            .arg(descr.data(PackDescription::ShortDescription).toString())
            .arg(tkTr(Trans::Constants::VERSION))
            .arg(descr.data(PackDescription::Version).toString())
            .arg(tkTr(Trans::Constants::LAST_MODIFICATION))
            .arg(descr.data(PackDescription::LastModificationDate).toDate().toString("dd MM yyyy"))
            .arg(tkTr(Trans::Constants::AUTHOR))
            .arg(descr.data(PackDescription::Author).toString())
            .arg(tkTr(Trans::Constants::VENDOR))
            .arg(pack.vendor())
            .arg(tkTr(Trans::Constants::DATA_TYPE))
            .arg(pack.dataTypeName())
            ;

    // Add description
    summary += descr.data(PackDescription::HtmlDescription).toString();

    // Add update informations
    bool isUpdate = d->m_PackModel->index(packId, PackModel::IsAnUpdate).data().toBool();
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
    QString zipPath = descr.data(PackDescription::UnzipToPath).toString();
    if (core().containsPathTag(zipPath))
        zipPath = core().replacePathTag(zipPath);
    QString file = QString("<span style=\"font-weight:bold\">%1</span><br />"
                           "<table border=1 cellpadding=0 cellspacing=0 width=100%>"
                           "<tr><td>%2</td><td>%3</td></tr>"
                           "<tr><td>MD5</td><td>%4</td></tr>"
                           "<tr><td>SHA1</td><td>%5</td></tr>"
                           "<tr><td>%6</td><td>%7</td></tr>"
                           "</table>")
            .arg(tr("File specification"))
            .arg(tr("File name or URL:"))
            .arg(descr.data(PackDescription::AbsFileName).toString())
            .arg(descr.data(PackDescription::Md5).toString())
            .arg(descr.data(PackDescription::Sha1).toString())
            .arg(tr("Unzip pack to path"))
            .arg(zipPath)
            ;

    summary += file;

    d->ui->packName->setText(descr.data(PackDescription::Label).toString());
    d->ui->packSummary->setText(summary);
}

void ServerPackEditor::onPackIndexActivated(const QModelIndex &index, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    populatePackView(index.row());
}

void ServerPackEditor::serverActionTriggered(QAction *a)
{
    if (a==d->aServerRefresh) {
        refreshServerContent();
    } if (a==d->aServerAdd) {
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
    } else if (a==d->aServerRemove) {
        if (!d->ui->serverView->selectionModel()->hasSelection())
            return;
        int row = d->ui->serverView->selectionModel()->currentIndex().row();
        serverManager()->removeServerAt(row);
    } else if (a==d->aServerEdit) {
        d->ui->stackedWidget->setCurrentWidget(d->ui->pageServers);
        d->processToolBar(::SERVER_MODE);
    } else if (a==d->aServerPackEditor) {
        d->ui->stackedWidget->setCurrentWidget(d->ui->pagePacks);
        d->processToolBar(::PACK_MODE);
    }
}

/** Start to process the user pack modification. */
void ServerPackEditor::processPacks()
{
    // Apply pack model changes
    if (!d->m_PackModel->isDirty())
        return;
    // Run Pack Dialog
//    PackProcessDialog dlg;
    PackWizard dlg;
    dlg.setPackToProcess(d->m_PackModel->packageToInstall(), d->m_PackModel->packageToUpdate(), d->m_PackModel->packageToRemove());
    if (dlg.exec()==QDialog::Rejected) {
        return;
    }
    packManager()->installedPack(true);
    d->m_PackModel->updateModel();
}

void ServerPackEditor::retranslate()
{
    d->aServerRefresh->setText(tr("Refresh datapack servers"));
    d->aServerEdit->setText(tr("Server editor"));
    d->aServerAdd->setText(tr("Add a server"));
    d->aServerRemove->setText(tr("Remove a server"));
    d->aServerPackEditor->setText(tr("Server and pack editor"));
    d->aProcess->setText(tr("Process changes"));
}

void ServerPackEditor::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        d->ui->retranslateUi(this);
        retranslate();
    }
}

void ServerPackEditor::on_listWidgetMenu_currentRowChanged(int row)
{
    Q_UNUSED(row);
    if (d->ui->listWidgetMenu->currentItem() == d->m_datapacksItem)
        d->ui->stackedWidget->setCurrentWidget(d->ui->pagePacks);
    else if (d->ui->listWidgetMenu->currentItem() == d->m_serversItem)
        d->ui->stackedWidget->setCurrentWidget(d->ui->pageServers);
}

void ServerPackEditor::serverCurrentChanged(const QModelIndex &c, const QModelIndex &p)
{
    Q_UNUSED(p);
    d->m_ServerMapper->setCurrentIndex(c.row());
}
