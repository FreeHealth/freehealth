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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DataPack::ServerPackEditor
 * Widget used to modify the server list and the installed packs.
*/

#include "serverpackeditor.h"
#include "addserverdialog.h"

#include <utils/global.h>
#include <utils/widgets/htmldelegate.h>
#include <utils/widgets/segmentedbutton.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_spashandupdate.h>

#include <datapackutils/datapackcore.h>
#include <datapackutils/servermanager.h>
#include <datapackutils/packmanager.h>
#include <datapackutils/packcategoriesmodel.h>
#include <datapackutils/packmodel.h>
#include <datapackutils/servermodel.h>
#include <datapackutils/widgets/packwizard.h>

#include <QToolBar>
#include <QProgressDialog>
#include <QProgressBar>
#include <QListWidgetItem>
#include <QDataWidgetMapper>
#include <QPushButton>
#include <QSpacerItem>

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

const char *const ICON_INSTALL = "install-package.png";

const char *const TITLE_CSS = "text-indent:5px;padding:5px;font-weight:bold;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))";

const char *const CSS =
        "QTreeView::item:hover {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
        "}"
        "QTreeView::item:selected:active{"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        "QTreeView::item:selected:!active {"
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
    ServerPackEditorPrivate(ServerPackEditor *parent) :
        ui(new Ui::ServerPackEditor),
        m_PackModel(0),
        m_PackCategoriesModel(0),
        m_serverModel(0),
        aServerRefresh(0), aServerEdit(0), aServerRemove(0), aServerAdd(0),
        aPackRefresh(0), aPackApply(0),
        m_ToolBarPacks(0),
        bServer(0), bPack(0),
        m_ServerMapper(0),
        _segmented(0), _segPack(0), _segServer(0),
        _toolbarCurrentMode(-1),
        _progressDialog(0),
        q(parent)
    {}

    void createActions()
    {
        // Create server actions
        QActionGroup *srvgr = new QActionGroup(q);
        QAction *a = aServerRefresh = new QAction(q);
        a->setObjectName("aServerRefresh");
        a->setIcon(icon(::ICON_SERVER_REFRESH, DataPack::DataPackCore::MediumPixmaps));
        a = aServerEdit = new QAction(q);
        a->setObjectName("aServerEdit");
        a->setIcon(icon(::ICON_SERVER_EDIT, DataPack::DataPackCore::MediumPixmaps));
        a = aServerAdd = new QAction(q);
        a->setObjectName("aInstall");
        a->setIcon(icon(::ICON_SERVER_ADD, DataPack::DataPackCore::MediumPixmaps));
        a = aServerRemove = new QAction(q);
        a->setObjectName("aServerRemove");
        a->setIcon(icon(::ICON_SERVER_REMOVE, DataPack::DataPackCore::MediumPixmaps));
        QObject::connect(srvgr, SIGNAL(triggered(QAction*)), q, SLOT(serverActionTriggered(QAction *)));

        // Create pack actions
        a = aPackRefresh = new QAction(q);
        a->setObjectName("aPackRefresh");
        a->setIcon(icon(::ICON_SERVER_REFRESH, DataPack::DataPackCore::MediumPixmaps));
        a = aPackApply = new QAction(q);
        a->setObjectName("aPackApply");
        a->setIcon(icon(::ICON_INSTALL, DataPack::DataPackCore::MediumPixmaps));
        QObject::connect(aPackRefresh, SIGNAL(triggered()), q, SLOT(refreshPacks()));
        QObject::connect(aPackApply, SIGNAL(triggered()), q, SLOT(processPacks()));
    }

    void createToolbar()
    {
        m_ToolBarPacks = new QToolBar(q);
        m_ToolBarPacks->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        // Create the segmented button with server/pack
        bServer = new QPushButton(q);
        bServer->setCheckable(true);
        bPack = new QPushButton(q);
        bPack->setCheckable(true);
        _segmented = new Utils::SegmentedButton(q);
        _segmented->setFirstButton(bServer);
        _segmented->setLastButton(bPack);
        _segmented->setAutoExclusive(true);
        QWidget *w1 = new QWidget(q);
        w1->setMinimumSize(20,20);
        w1->setMaximumSize(20,20);
        m_ToolBarPacks->addWidget(w1);
        m_ToolBarPacks->addWidget(_segmented);
        w1 = new QWidget(q);
        w1->setMinimumSize(20,20);
        w1->setMaximumSize(20,20);
        m_ToolBarPacks->addWidget(w1);
        m_ToolBarPacks->addSeparator();

        processToolBar(::PACK_MODE);

        m_ToolBarPacks->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        ui->toolbarLayout->addWidget(m_ToolBarPacks);

        QObject::connect(m_ToolBarPacks, SIGNAL(actionTriggered(QAction*)), q, SLOT(serverActionTriggered(QAction*)));
        QObject::connect(bPack, SIGNAL(clicked()), q, SLOT(switchToPackView()));
        QObject::connect(bServer, SIGNAL(clicked()), q, SLOT(switchToServerView()));
    }

    void processToolBar(int mode)
    {
        if (_toolbarCurrentMode==mode)
            return;
        _toolbarCurrentMode = mode;
        if (mode==::SERVER_MODE) {
            m_ToolBarPacks->removeAction(aPackRefresh);
            m_ToolBarPacks->removeAction(aPackApply);
            m_ToolBarPacks->addAction(aServerRefresh);
            m_ToolBarPacks->addAction(aServerAdd);
            m_ToolBarPacks->addAction(aServerRemove);
        } else if (mode==::PACK_MODE) {
            m_ToolBarPacks->removeAction(aServerRefresh);
            m_ToolBarPacks->removeAction(aServerAdd);
            m_ToolBarPacks->removeAction(aServerRemove);
            m_ToolBarPacks->addAction(aPackRefresh);
            m_ToolBarPacks->addAction(aPackApply);
        }
    }

public:
    Ui::ServerPackEditor *ui;
    PackModel *m_PackModel;
    PackCategoriesModel *m_PackCategoriesModel;
    ServerModel *m_serverModel;
    QAction *aServerRefresh, *aServerEdit, *aServerRemove, *aServerAdd;
    QAction *aPackRefresh, *aPackApply;
    QToolBar *m_ToolBarPacks;
    QPushButton *bServer, *bPack;
    QDataWidgetMapper *m_ServerMapper;
    Utils::SegmentedButton *_segmented, *_segPack, *_segServer;
    int _toolbarCurrentMode;
    QProgressDialog *_progressDialog; // used for server content refreshing

private:
    ServerPackEditor *q;
};
} // Internal
} // DataPack

ServerPackEditor::ServerPackEditor(QWidget *parent) :
    QWidget(parent),
    d(new Internal::ServerPackEditorPrivate(this))
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
        d->ui->packSplitter->setStretchFactor(0, 1);
        d->ui->packSplitter->setStretchFactor(1, 2);
        d->ui->serverSplitter->setStretchFactor(0, 1);
        d->ui->serverSplitter->setStretchFactor(1, 2);
        d->ui->packViewSplitter->setStretchFactor(0, 1);
        d->ui->packViewSplitter->setStretchFactor(1, 2);
    }

    // Manage pack model/view
    d->m_PackModel = new PackModel(this);
    d->m_PackModel->setPackCheckable(true);
    d->m_PackModel->setInstallChecker(true);
    d->ui->packView->setModel(d->m_PackModel);
    d->ui->packView->setModelColumn(PackModel::Label);
    d->m_PackCategoriesModel = new PackCategoriesModel(this);
    d->ui->packCategoriesView->setModel(d->m_PackCategoriesModel);
    d->ui->packCategoriesView->header()->hide();
    d->ui->packCategoriesView->setStyleSheet(::CSS);
    connect(d->ui->packCategoriesView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onPackCategoriesChanged(QModelIndex, QModelIndex)));

    Utils::HtmlDelegate *packdelegate = new Utils::HtmlDelegate(this);
    d->ui->packView->setItemDelegate(packdelegate);
    d->ui->packView->setStyleSheet(::CSS);
    //    d->ui->packView->header()->hide();
    d->ui->packView->setAlternatingRowColors(true);
    d->ui->packView->setUniformItemSizes(false);

    // server view/model/delegate
    Utils::HtmlDelegate *serverdelegate = new Utils::HtmlDelegate(this);
    d->m_serverModel = new ServerModel(this);
    d->m_serverModel->initialize();
    d->ui->serverListView->setModel(d->m_serverModel);
    d->ui->serverListView->setModelColumn(ServerModel::HtmlLabel);
    d->ui->serverListView->setItemDelegate(serverdelegate);
    d->ui->serverListView->setStyleSheet(::CSS);
    d->ui->serverListView->setAlternatingRowColors(true);
    d->ui->serverListView->setResizeMode(QListView::Adjust);
    d->ui->serverListView->setUniformItemSizes(false);

    // Manage central view
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize()+1);
    d->ui->packName->setFont(bold);
    d->ui->packName->setStyleSheet(::TITLE_CSS);
    d->ui->serverName->setFont(bold);
    d->ui->serverName->setStyleSheet(::TITLE_CSS);

    d->createActions();
    d->createToolbar();
    switchToPackView();
//    createServerDataWidgetMapper();
    retranslate();

    d->ui->stackedWidget->setCurrentWidget(d->ui->packPage);
    connect(d->ui->packView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onPackIndexActivated(QModelIndex,QModelIndex)));
    connect(d->ui->serverListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(serverCurrentChanged(QModelIndex,QModelIndex)));

    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), this, SLOT(selectFirstRow()));
    // file://Users/eric/Desktop/Programmation/freemedforms/global_resources/datapacks/default/

    //refreshServerContent();
    //selectFirstRow();
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
    if (d->_progressDialog) {
        delete d->_progressDialog;
        d->_progressDialog = 0;
    }
    d->_progressDialog = new QProgressDialog(this);
    QProgressBar *bar = new QProgressBar(d->_progressDialog);
    bar->setValue(0);
    bar->setRange(0, 0);
    d->_progressDialog->setBar(bar);
    d->_progressDialog->setLabelText(tr("Updating server information"));
    d->_progressDialog->setModal(true);
    d->_progressDialog->show();
    connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), this, SLOT(onRefreshServerDone()), Qt::UniqueConnection);
    // TODO: Connect the cancel button
    //connect(d->_progressDialog, SIGNAL(canceled()), &core(), SLOT(stopJobsAndClearQueues()));
    //connect(d->_progressDialog, SIGNAL(canceled()), d->_progressDialog, SLOT(deleteLater()));
    serverManager()->getAllDescriptionFile(bar);
//    dlg.exec();
    return true;
}

void ServerPackEditor::onRefreshServerDone()
{
    if (!d->_progressDialog)
        return;
    d->_progressDialog->setValue(d->_progressDialog->maximum());
    delete d->_progressDialog;
    d->_progressDialog = 0;
    d->aServerRefresh->setEnabled(true);
}

bool ServerPackEditor::submitChanges()
{
    return true;
}


//void ServerPackEditor::createServerDataWidgetMapper()
//{
//    d->m_ServerMapper = new QDataWidgetMapper(this);
//    d->m_ServerMapper->setModel(d->m_serverModel);
//    d->m_ServerMapper->addMapping(d->ui->serverLabel, ServerModel::PlainTextLabel, "text");
//    d->m_ServerMapper->addMapping(d->ui->serverVersion, ServerModel::Version, "text");
//    d->m_ServerMapper->addMapping(d->ui->serverAuthors, ServerModel::Authors, "text");
//    d->m_ServerMapper->addMapping(d->ui->serverCreationDate, ServerModel::CreationDate, "text");
//    d->m_ServerMapper->addMapping(d->ui->serverVendor, ServerModel::Vendor, "text");
//    d->m_ServerMapper->addMapping(d->ui->serverDescription, ServerModel::HtmlDescription, "html");
//    d->m_ServerMapper->setCurrentIndex(1);
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

    // Add update information
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
    Server server = serverManager()->getServerForPack(pack);
    QString file = QString("<span style=\"font-weight:bold\">%1</span><br />"
                           "<table border=1 cellpadding=0 cellspacing=0 width=100%>"
                           "<tr><td>%2</td><td>%3</td></tr>"
                           "<tr><td>%4</td><td>%5</td></tr>"
                           "<tr><td>MD5</td><td>%6</td></tr>"
                           "<tr><td>SHA1</td><td>%7</td></tr>"
                           "<tr><td>%8</td><td>%9</td></tr>"
                           "</table>")
            .arg(tr("File specification"))
            .arg(tkTr(Trans::Constants::SERVER))
            .arg(server.url())
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

void ServerPackEditor::switchToPackView()
{
    d->ui->stackedWidget->setCurrentWidget(d->ui->packPage);
    d->bPack->setChecked(true);
    d->processToolBar(::PACK_MODE);
}

void ServerPackEditor::switchToServerView()
{
    d->ui->stackedWidget->setCurrentWidget(d->ui->serverPage);
    d->bServer->setChecked(true);
    d->processToolBar(::SERVER_MODE);
}

void ServerPackEditor::onPackCategoriesChanged(const QModelIndex &index, const QModelIndex &)
{
    if (!index.isValid())
        return;
    // Filter the packmodel according to the selected category
    const QString &vendor = d->m_PackCategoriesModel->vendor(index);
    const QList<Pack::DataType> &type = d->m_PackCategoriesModel->datatype(index);
    d->m_PackModel->filter(vendor, type);
    // Clear the view
    d->ui->packName->clear();
    d->ui->packSummary->clear();
    // Select the first available row
    d->ui->packView->setCurrentIndex(d->m_PackModel->index(0,0));
    d->ui->packView->selectionModel()->select(d->m_PackModel->index(0,0), QItemSelectionModel::SelectCurrent);
    onPackIndexActivated(d->m_PackModel->index(0,0), QModelIndex());
}

void ServerPackEditor::onPackIndexActivated(const QModelIndex &index, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (index.isValid())
        populatePackView(index.row());
}

void ServerPackEditor::serverActionTriggered(QAction *a)
{
    if (a==d->aServerRefresh) {
        d->aServerRefresh->setEnabled(false);
        refreshServerContent();
    } if (a==d->aServerAdd) {
        AddServerDialog dlg(this);
        Server server;
        dlg.setServer(server);
        if (dlg.exec()==QDialog::Accepted) {
            dlg.submitTo(&server);
            serverManager()->addServer(server);
            // TODO: manage progress dialog cancellation
            QProgressDialog dlg(this);
            dlg.setLabelText(tr("Downloading server information"));
            dlg.setModal(true);
            connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), &dlg, SLOT(accept()));
//            connect(serverManager(), SIGNAL(allServerDescriptionAvailable()), dlg, SLOT(close()));
            serverManager()->getServerDescription(serverManager()->serverCount() - 1);
            dlg.exec();
        }
    } else if (a==d->aServerRemove) {
        if (!d->ui->serverListView->selectionModel()->hasSelection())
            return;
        int row = d->ui->serverListView->selectionModel()->currentIndex().row();
        serverManager()->removeServerAt(row);
    } else if (a==d->aServerEdit) {
        switchToServerView();
    }
}

void ServerPackEditor::refreshPacks()
{
    refreshServerContent();
}

/** Start to process the user pack modification. */
void ServerPackEditor::processPacks()
{
    // Apply pack model changes
    if (!d->m_PackModel->isDirty())
        return;
    // Run Pack Dialog
    PackWizard dlg;
    dlg.setPackToProcess(d->m_PackModel->packageToInstall(), d->m_PackModel->packageToUpdate(), d->m_PackModel->packageToRemove());
    if (dlg.exec()==QDialog::Rejected) {
        return;
    }
    // Refresh the cached installed pack list from the packmanager
    packManager()->installedPack(true);
    d->m_PackModel->updateModel();
}

void ServerPackEditor::populateServerView(const int serverId)
{
    const Server &server = serverManager()->getServerAt(serverId);
    const ServerDescription &descr = server.description();
    QString summary;

    // short description, version date and author
    if (!server.uuid().isEmpty() && !server.version().isEmpty()) {
        summary = QString("<p style=\"font-weight:bold;font-size:large;\">%1</p>"
                          "<p style=\"font-size:small;margin-left:20px;color:gray\">"
                          "%2: %3<br />"
                          "%4: %5<br />"
                          "%6: %7<br />"
                          "%8: %9<br />"
                          "%10: %11<br />"
                          "%12: %13<br />"
                          "</p>"
                          )
                .arg(descr.data(ServerDescription::ShortDescription).toString())
                .arg(tkTr(Trans::Constants::VERSION))
                .arg(descr.data(ServerDescription::Version).toString())
                .arg(tkTr(Trans::Constants::LAST_MODIFICATION))
                .arg(descr.data(ServerDescription::LastModificationDate).toDate().toString("dd MM yyyy"))
                .arg(tkTr(Trans::Constants::AUTHOR))
                .arg(descr.data(ServerDescription::Author).toString())
                .arg(tkTr(Trans::Constants::VENDOR))
                .arg(descr.data(ServerDescription::Vendor).toString())
                .arg(tr("Native URL"))
                .arg(server.nativeUrl())
                .arg(tkTr(Trans::Constants::RECOMMENDED_UPDATE_FREQUENCY))
                .arg(Trans::ConstantTranslations::checkUpdateLabel(server.recommendedUpdateFrequency()))
                ;
    } else {
        summary = QString("<p style=\"font-weight:bold;\">%1</p>"
                          "<p style=\"font-weight:bold;\">%2: %3</p>"
                          )
                .arg(tr("No information available. Please check the URL of the server."))
                .arg(tr("Native URL"))
                .arg(server.nativeUrl())
                ;
    }

    // Add description
    summary += descr.data(ServerDescription::HtmlDescription).toString();

    // Add update information
//    bool isUpdate = d->m_PackModel->index(packId, PackModel::IsAnUpdate).data().toBool();
//    if (isUpdate) {
//        QString up = QString("<p style=\"font-size:normal;margin-left:10px;color:darkblue\">%1</p><br />")
//                     .arg(descr.htmlUpdateInformationForVersion("0.0.0"));
//        up.prepend(QString("<span style=\"font-weight:bold;margin:0px;\">%1</span>").arg(tr("Update information")));
//        summary += up;
//    }

//    // Add file specifications
//    QString zipPath = descr.data(PackDescription::UnzipToPath).toString();
//    if (core().containsPathTag(zipPath))
//        zipPath = core().replacePathTag(zipPath);
//    QString file = QString("<span style=\"font-weight:bold\">%1</span><br />"
//                           "<table border=1 cellpadding=0 cellspacing=0 width=100%>"
//                           "<tr><td>%2</td><td>%3</td></tr>"
//                           "<tr><td>MD5</td><td>%4</td></tr>"
//                           "<tr><td>SHA1</td><td>%5</td></tr>"
//                           "<tr><td>%6</td><td>%7</td></tr>"
//                           "</table>")
//            .arg(tr("File specification"))
//            .arg(tr("File name or URL:"))
//            .arg(descr.data(PackDescription::AbsFileName).toString())
//            .arg(descr.data(PackDescription::Md5).toString())
//            .arg(descr.data(PackDescription::Sha1).toString())
//            .arg(tr("Unzip pack to path"))
//            .arg(zipPath)
//            ;

//    summary += file;

    d->ui->serverName->setText(descr.data(ServerDescription::Label).toString());
    d->ui->serverContent->setText(summary);
}

void ServerPackEditor::serverCurrentChanged(const QModelIndex &c, const QModelIndex &p)
{
    Q_UNUSED(p);
    populateServerView(c.row());
}

void ServerPackEditor::selectFirstRow()
{
    d->ui->packCategoriesView->setCurrentIndex(d->m_PackCategoriesModel->index(0,0));
    d->ui->packCategoriesView->selectionModel()->select(d->m_PackCategoriesModel->index(0,0), QItemSelectionModel::SelectCurrent);
    onPackCategoriesChanged(d->m_PackCategoriesModel->index(0,0), QModelIndex());
    d->ui->packView->setCurrentIndex(d->m_PackModel->index(0,0));
    d->ui->packView->selectionModel()->select(d->m_PackModel->index(0,0), QItemSelectionModel::SelectCurrent);
    onPackIndexActivated(d->m_PackModel->index(0,0), QModelIndex());
    for(int i=0; i<d->m_PackCategoriesModel->rowCount(); ++i)
        d->ui->packCategoriesView->expand(d->m_PackCategoriesModel->index(i,0));

    d->ui->serverListView->setCurrentIndex(d->m_serverModel->index(0,0));
    d->ui->serverListView->selectionModel()->select(d->m_serverModel->index(0,0), QItemSelectionModel::SelectCurrent);
    populateServerView(0);
}

void ServerPackEditor::retranslate()
{
    d->aServerRefresh->setText(tr("Refresh datapack servers"));
    d->aServerEdit->setText(tr("Server editor"));
    d->aServerAdd->setText(tr("Add a server"));
    d->aServerRemove->setText(tr("Remove a server"));
    d->aPackRefresh->setText(tr("Refresh packs"));
    d->aPackApply->setText(tr("Apply changes"));
    d->bServer->setText(tkTr(Trans::Constants::SERVERS));
    d->bPack->setText(tkTr(Trans::Constants::PACKAGES));
    d->_segmented->computeSizes();
}

void ServerPackEditor::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        d->ui->retranslateUi(this);
        retranslate();
    }
}

