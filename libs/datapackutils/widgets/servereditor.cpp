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

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <datapackutils/core.h>
#include <datapackutils/servermanager.h>

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextDocument>

#include <QDebug>

#include "ui_servereditor.h"

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {
const char *const ICON_SERVER_CONNECTED = "connect_established.png";
const char *const ICON_SERVER_NOT_CONNECTED = "connect_no.png";
const char *const ICON_SERVER_ASKING_CONNECTION = "connect_creating.png";
const char *const ICON_PACKAGE = "package.png";

const int IS_SERVER = 1;
const int IS_PACK = 2;

const char * const TITLE_CSS = "text-indent:5px;padding:5px;font-weight:bold;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))";

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

class Delegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    int leftIndent = 0;
    QVariant decoration = index.data(Qt::DecorationRole);
    if (!decoration.isNull()) {
        QIcon icon = decoration.value<QIcon>();
        if (!icon.isNull()) {
            leftIndent = icon.actualSize(QSize(32,32)).width();
        }
    }

    QTextDocument doc;
    QString text = options.text;
    if (option.state & QStyle::State_Selected) {
        text.replace("color:gray", "color:lightgray");
        text.replace("color:black", "color:white");
    }
    doc.setHtml(text);

    /* Call this to get the focus rect and selection background. */
    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter, options.widget);

    /* Draw using our rich text document. */
    painter->translate(options.rect.left() + leftIndent, options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}

}  // End anonymous namespace

static inline DataPack::Core *core() {return DataPack::Core::instance();}
static inline ServerManager *serverManager() {return qobject_cast<ServerManager*>(core()->serverManager());}
static inline QIcon icon(const QString &name) { qWarning() << DataPack::Core::instance()->icon(name, DataPack::Core::MediumPixmaps); return QIcon(DataPack::Core::instance()->icon(name, DataPack::Core::MediumPixmaps));}

static void createServerModel(QStandardItemModel *model)
{
    ServerManager *manager = serverManager();
    QFont bold;
    bold.setBold(true);
    model->clear();
    QStandardItem *root = model->invisibleRootItem();
    for(int i = 0; i < manager->serverCount(); ++i) {
        const Server &s = manager->getServerAt(i);
        QString label = QString("<span style=\"color:black;font-weight:bold\">%1</span>")
                .arg(s.description().data(ServerDescription::Label).toString());

        if (s.isConnected()) {
            label += QString("<br /><span style=\"color:gray; font-size:small\">%2 (%3: %4)</span>")
                    .arg(tkTr(Trans::Constants::CONNECTED))
                    .arg(tkTr(Trans::Constants::LAST_CHECK))
                    .arg(s.lastChecked().toString(QLocale().dateFormat(QLocale::LongFormat)));
        } else {
            label += QString("<br /><small>%2</small>")
                    .arg(tkTr(Trans::Constants::NOT_CONNECTED));
        }

        label += QString("<br /><span style=\"color:gray; font-size:small\">%1 %2</span>")
                .arg(serverManager()->getPackForServer(s).count())
                .arg(tkTr(Trans::Constants::PACKAGES));

        QStandardItem *server = new QStandardItem(label);
        server->setData(::IS_SERVER);
        root->appendRow(server);

        if (s.isConnected()) {
            server->setIcon(icon(::ICON_SERVER_CONNECTED));
            // Add packdescriptions
        } else {
            server->setIcon(icon(::ICON_SERVER_NOT_CONNECTED));
        }
    }
}


static void createPackModel(const Server &server, QStandardItemModel *model)
{
    const QList<Pack> &packs = serverManager()->getPackForServer(server);
    model->clear();
    for(int i=0; i < packs.count(); ++i) {
        QString pack = QString("<span style=\"color:black;font-weight:bold\">%1</span><br />"
                               "<span style=\"color:gray; font-size:small\">%2: %3</span>")
                .arg(packs.at(i).description().data(PackDescription::Label).toString())
                .arg(tkTr(Trans::Constants::VERSION))
                .arg(packs.at(i).description().data(PackDescription::Version).toString());
        QStandardItem *packItem = new QStandardItem(pack);
        QString f = packs.at(i).description().data(PackDescription::GeneralIcon).toString();
        if (f.isEmpty()) {
            packItem->setIcon(icon(::ICON_PACKAGE));
        } else {
            f = f.remove("__theme__/");
            packItem->setIcon(icon(f));
        }
        packItem->setData(::IS_PACK);
        model->appendRow(packItem);
    }
}


ServerEditor::ServerEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerEditor)
{
    ui->setupUi(this);
    if (layout()) {
        layout()->setMargin(0);
        layout()->setSpacing(0);
    }

    m_ServerModel = new QStandardItemModel(this);
    m_ServerModel->setColumnCount(1);
    createServerModel(m_ServerModel);
    ui->treeView->setModel(m_ServerModel);
    ui->treeView->header()->hide();
    Delegate *delegate = new Delegate;
    ui->treeView->setItemDelegate(delegate);
    ui->treeView->setStyleSheet(::CSS);
    ui->treeView->expandAll();
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setRootIsDecorated(false);
    ui->treeView->setEditTriggers(QTreeView::NoEditTriggers);

    m_PackModel = new QStandardItemModel(this);
    m_PackModel->setColumnCount(1);
    ui->packView->setModel(m_PackModel);
    ui->packView->header()->hide();
//    Delegate *delegate = new Delegate;
    ui->packView->setItemDelegate(delegate);
    ui->packView->setStyleSheet(::CSS);
    ui->packView->expandAll();
    ui->packView->setRootIsDecorated(false);
    ui->packView->setAlternatingRowColors(true);
    ui->packView->setEditTriggers(QTreeView::NoEditTriggers);

    ui->stackedWidget->setCurrentIndex(0);
    QFont bold;
    bold.setBold(true);
    bold.setPointSize(bold.pointSize()+1);
    ui->serverName->setFont(bold);
    ui->packName->setFont(bold);
    ui->serverName->setStyleSheet(::TITLE_CSS);
    ui->packName->setStyleSheet(::TITLE_CSS);

    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onServerIndexActivated(QModelIndex,QModelIndex)));
    connect(ui->packView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onPackIndexActivated(QModelIndex,QModelIndex)));
}

ServerEditor::~ServerEditor()
{
    delete ui;
}

bool ServerEditor::submitChanges()
{
    return true;
}

void ServerEditor::populateServerView(const int serverId)
{
    const QString &format = QLocale().dateTimeFormat(QLocale::LongFormat);
    const Server &server = serverManager()->getServerAt(serverId);
    createPackModel(server, m_PackModel);
    ui->serverName->setText(server.description().data(ServerDescription::Label).toString());
    ui->shortServerDescription->setText(server.description().data(ServerDescription::ShortDescription).toString());
    ui->serverVersion->setText(server.description().data(ServerDescription::Version).toString());
    ui->serverDate->setText(server.description().data(ServerDescription::LastModificationDate).toDateTime().toString(format));
    ui->serverDescription->setText(server.description().data(ServerDescription::ShortDescription).toString());
    ui->lastCheck->setText(server.lastChecked().toString(format));
    ui->serverVendor->setText(server.description().data(ServerDescription::Vendor).toString());
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
    // Activate the Server Page
    ui->stackedWidget->setCurrentWidget(ui->serverPage);
    populateServerView(index.row());
}

void ServerEditor::onPackIndexActivated(const QModelIndex &index, const QModelIndex &previous)
{
    // Activate the Server Page
    ui->stackedWidget->setCurrentWidget(ui->packPage);
    int serverId = ui->treeView->selectionModel()->currentIndex().row();
    populatePackView(serverId, index.row());
}
